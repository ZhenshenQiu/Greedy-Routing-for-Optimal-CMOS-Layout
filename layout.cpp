//
//  gate.h
//  
//
//  Created by Zhenshen Qiu on 9/7/13.
//
//
#include "layout.h"


////////////////////////////////////////initialize the class members
booExpression::booExpression()
{
    root = NULL;
    Phead = NULL;
    Nhead = NULL;
    useContE=0;
    useContE2=0;
    useContO=1;
    useContO2=1;
    polyCont=0;
	expLength = 0;
	polySilicon[0]=' ';
	boolean[0]=" ";
    legal=true;//initialize the input legal
    gateNum=1;//initialize the root number, it must be one
    inputControl();//initialize the root expression
    initGateTree(boolean[0]);//boolean;
}

////////////////////////////////////////initialize the class members
booExpression::booExpression(string s)
{
    root = NULL;
    Phead = NULL;
    Nhead = NULL;
    useContE=0;
    useContE2=0;
    useContO=1;
    useContO2=1;
    polyCont=0;
	polySilicon[0]=' ';
	boolean[0]=" ";
    legal=true;//initialize the input legal
    gateNum=1;//initialize the root number, it must be one
    inputControl();//initialize the root expression
    initGateTree(s);//boolean;
}

///////////////////////////////////////////////////////////////
void booExpression::generate(void)
{
	 //********************* define one class variable ******************//
    //booExpression myBoolean;
    
    //********** reshape the gate tree by the layout algorithm **************//
    reshape(root);
    
    //********************** diaplay the polysilicon sequence  **************//
    displayPolySilicon();
    
    //************ pre-create the transistor netlist information  **********//
    createPmosNet(root, Phead, OUTT, VDD);//n-mos
    createNmosNet(root, Nhead, OUTT, GND);//n-mos
    
    //************** rebuild the netlist informations ********************//
    redoUseList();
    
    //********************** output the P-mos netlist ******************//
    cout<<"the P-mos netlist is:"<<endl;
    cout<<"Gate"<<"\t"<<"Drain"<<"\t"<<"Sourse"<<endl;
    redoPmosNet(root, Phead, OUTT, VDD);//n-mos
    cout<<endl;
    
    //********************** output the N-mos netlist ******************//
    cout<<"the N-mos netlist is:"<<endl;
    cout<<"Gate"<<"\t"<<"Drain"<<"\t"<<"Sourse"<<endl;
    redoNmosNet(root, Nhead, OUTT, GND);//n-mos
    cout<<endl;
}

/////////////////////////////////////
net *booExpression::redoPmosNet(gate *tempRoot, net *tempHead, int dra, int sou)//p-mos
{
    int i;
    int h;
    int cont;
    int newNodes[MAX];
    int help=0;
    net *newNet=new net;//create a new net
    if(tempHead==NULL)//create the original drain and source
    {
        newNet->drain=OUTT;
        newNet->source=VDD;
        newNet->tran=tempRoot->expression;
        newNet->num=tempRoot->Number;
        newNet->mos='P';
        tempHead=newNet;//after creating the original diagram
        Phead=newNet;
    }
    if(tempRoot->Number!=0)//there's child lines
    {
        if(tempRoot->Symbol=='+')//for p-mos, it is serial
        {
            newNodes[0]=dra;//create new nodes
            newNodes[tempRoot->realNumber]=sou;
            //cout<<tempRoot->realNumber<<endl;
            //cout<<newNodes[0]<<endl;
            for(i=1;i<tempRoot->realNumber;i++)
            {
                newNodes[i]=dra+(sou-dra)/tempRoot->realNumber*i;//create new nodes
                if(!(newNodes[i]%2))
                    newNodes[i]=newNodes[i]+1;
                newNodes[i]=changeNum2(newNodes[i]);
                //cout<<newNodes[i]<<endl;
            }
            //cout<<newNodes[tempRoot->realNumber]<<endl;
            for(i=0;i<tempRoot->Number;i++)
            {
                if(tempRoot->nextPort[i]->expression=="%")
                {
                    for(h=tempRoot->Number;h>0;h--)
                        newNodes[h]=newNodes[h-1];
                }
                else
                    tempHead->nextTran[i]=redoPmosNet(tempRoot->nextPort[i], tempHead, newNodes[i], newNodes[i+1]);
            }
        }
        else//for p-mos, it is parellel
        {
            for(i=0;i<tempRoot->Number;i++)
            {
                if(i>0)//afte the first line, each later lines should all do reverse
                {
                    help=dra;
                    dra=sou;
                    sou=help;
                }
                if(tempRoot->nextPort[i]->expression!="%")
                    tempHead->nextTran[i]=redoPmosNet(tempRoot->nextPort[i], tempHead, dra, sou);
            }
        }
        
    }
    else//it is a port, quit the recursion and name the transistor
    {
        newNet->tran=tempRoot->expression;
        newNet->num=tempRoot->Number;
        newNet->mos='P';
        newNet->drain=dra;
        newNet->source=sou;
        if(newNet->tran!="%")//do not show the pseudo line
        {
            for(cont=0;cont<useContO-1;cont++)
            {
                if(newNet->drain==useList[cont]&&newNet->drain!=1)
                {
                    newNet->drain=cont+2;
                }
                if(newNet->source==useList[cont]&&newNet->source!=1)
                {
                    newNet->source=cont+2;
                }
            }
			if(newNet->drain==100 && newNet->source==1)
			{
				vector<string> temp;
				temp.push_back(newNet->tran);
				temp.push_back("OUT");
				temp.push_back("VDD");
				PnetOrder.push_back(temp);
			}
            else if(newNet->drain==1 && newNet->source==100)
			{
				vector<string> temp;
				temp.push_back(newNet->tran);
				temp.push_back("VDD");
				temp.push_back("OUT");
				PnetOrder.push_back(temp);
			}
            else if(newNet->drain==100)
			{
				string str;
				str.push_back(newNet->mos);
				ostringstream ostr;
				ostr<<newNet->source;
				string s=ostr.str();
				str += s;
				vector<string> temp;
				temp.push_back(newNet->tran);
				temp.push_back("OUT");
				temp.push_back(str);
				PnetOrder.push_back(temp);
			}
            else if(newNet->drain==1)
			{
				string str;
				str.push_back(newNet->mos);
				ostringstream ostr;
				ostr<<newNet->source;
				string s=ostr.str();
				str += s;
				vector<string> temp;
				temp.push_back(newNet->tran);
				temp.push_back("VDD");
				temp.push_back(str);
				PnetOrder.push_back(temp);
			}
            else if(newNet->source==1)
			{
				string str;
				str.push_back(newNet->mos);
				ostringstream ostr;
				ostr<<newNet->drain;
				string s=ostr.str();
				str += s;
				vector<string> temp;
				temp.push_back(newNet->tran);
				temp.push_back(str);
				temp.push_back("VDD");
				PnetOrder.push_back(temp);
			}
            else if(newNet->source==100)
			{
				string str;
				str.push_back(newNet->mos);
				ostringstream ostr;
				ostr<<newNet->drain;
				string s=ostr.str();
				str += s;
				vector<string> temp;
				temp.push_back(newNet->tran);
				temp.push_back(str);
				temp.push_back("OUT");
				PnetOrder.push_back(temp);
			}
            else
			{
				string str1;
				str1.push_back(newNet->mos);
				ostringstream ostr1;
				ostr1<<newNet->drain;
				string s1=ostr1.str();
				str1 += s1;
				string str2;
				str2.push_back(newNet->mos);
				ostringstream ostr2;
				ostr2<<newNet->source;
				string s2=ostr2.str();
				str2 += s2;
				vector<string> temp;
				temp.push_back(newNet->tran);
				temp.push_back(str1);
				temp.push_back(str2);
				PnetOrder.push_back(temp);
			}
			////////////////////////////////////////////////////////////// netlist
            if(newNet->drain<newNet->source)//disregard the eular path
            {
                help=newNet->drain;
                newNet->drain=newNet->source;
                newNet->source=help;
            }
            if(newNet->drain==100 && newNet->source==1)
			{
				vector<string> temp;
				temp.push_back(newNet->tran);
				temp.push_back("OUT");
				temp.push_back("VDD");
				Pnet.push_back(temp);
                cout<<newNet->tran<<"\t"<<"OUT"<<"\t"<<"VDD"<<endl;
			}
            else if(newNet->drain==1 && newNet->source==100)
			{
				vector<string> temp;
				temp.push_back(newNet->tran);
				temp.push_back("VDD");
				temp.push_back("OUT");
				Pnet.push_back(temp);
                cout<<newNet->tran<<"\t"<<"VDD"<<"\t"<<"OUT"<<endl;
			}
            else if(newNet->drain==100)
			{
				string str;
				str.push_back(newNet->mos);
				ostringstream ostr;
				ostr<<newNet->source;
				string s=ostr.str();
				str += s;
				vector<string> temp;
				temp.push_back(newNet->tran);
				temp.push_back("OUT");
				temp.push_back(str);
				Pnet.push_back(temp);
                cout<<newNet->tran<<"\t"<<"OUT"<<"\t"<<newNet->mos<<newNet->source<<endl;
			}
            else if(newNet->drain==1)
			{
				string str;
				str.push_back(newNet->mos);
				ostringstream ostr;
				ostr<<newNet->source;
				string s=ostr.str();
				str += s;
				vector<string> temp;
				temp.push_back(newNet->tran);
				temp.push_back("VDD");
				temp.push_back(str);
				Pnet.push_back(temp);
                cout<<newNet->tran<<"\t"<<"VDD"<<"\t"<<newNet->mos<<newNet->source<<endl;
			}
            else if(newNet->source==1)
			{
				string str;
				str.push_back(newNet->mos);
				ostringstream ostr;
				ostr<<newNet->drain;
				string s=ostr.str();
				str += s;
				vector<string> temp;
				temp.push_back(newNet->tran);
				temp.push_back(str);
				temp.push_back("VDD");
				Pnet.push_back(temp);
                cout<<newNet->tran<<"\t"<<newNet->mos<<newNet->drain<<"\t"<<"VDD"<<"\t"<<endl;
			}
            else if(newNet->source==100)
			{
				string str;
				str.push_back(newNet->mos);
				ostringstream ostr;
				ostr<<newNet->drain;
				string s=ostr.str();
				str += s;
				vector<string> temp;
				temp.push_back(newNet->tran);
				temp.push_back(str);
				temp.push_back("OUT");
				Pnet.push_back(temp);
                cout<<newNet->tran<<"\t"<<newNet->mos<<newNet->drain<<"\t"<<"OUT"<<endl;
			}
            else
			{
				string str1;
				str1.push_back(newNet->mos);
				ostringstream ostr1;
				ostr1<<newNet->drain;
				string s1=ostr1.str();
				str1 += s1;
				string str2;
				str2.push_back(newNet->mos);
				ostringstream ostr2;
				ostr2<<newNet->source;
				string s2=ostr2.str();
				str2 += s2;
				vector<string> temp;
				temp.push_back(newNet->tran);
				temp.push_back(str1);
				temp.push_back(str2);
				Pnet.push_back(temp);
                cout<<newNet->tran<<"\t"<<newNet->mos<<newNet->drain<<"\t"<<newNet->mos<<newNet->source<<endl;
			}
        }
    }
    return tempHead;
}
/////////////////////////////////////
net *booExpression::redoNmosNet(gate *tempRoot, net *tempHead, int dra, int sou)//n-mos
{
    int i;
    int h;
    int cont;
    int newNodes[MAX];
    int help=0;
    net *newNet=new net;//create a new net
    if(tempHead==NULL)//create the original drain and source
    {
        newNet->drain=OUTT;
        newNet->source=GND;
        newNet->tran=tempRoot->expression;
        newNet->num=tempRoot->Number;
        newNet->mos='N';
        tempHead=newNet;//after creating the original diagram
        Phead=newNet;
    }
    if(tempRoot->Number!=0)//there's child lines
    {
        if(tempRoot->Symbol=='*')//for n-mos, it is serial
        {
            newNodes[0]=dra;//create new nodes
            newNodes[tempRoot->realNumber]=sou;
            for(i=1;i<tempRoot->realNumber;i++)
            {
                newNodes[i]=dra+(sou-dra)/tempRoot->realNumber*i;//create new nodes
                if(newNodes[i]%2)
                    newNodes[i]=newNodes[i]+1;
                newNodes[i]=changeNum2(newNodes[i]);
                //cout<<newNodes[i]<<endl;
            }
            //cout<<newNodes[tempRoot->realNumber]<<endl;
            for(i=0;i<tempRoot->Number;i++)
            {
                if(tempRoot->nextPort[i]->expression=="%")
                {
                    for(h=tempRoot->Number;h>0;h--)
                        newNodes[h]=newNodes[h-1];
                }
                else
                    tempHead->nextTran[i]=redoNmosNet(tempRoot->nextPort[i], tempHead, newNodes[i], newNodes[i+1]);
            }
        }
        else//for n-mos, it is parellel
        {
            for(i=0;i<tempRoot->Number;i++)
            {
                if(i>0)//afte the first line, each later lines should all do reverse
                {
                    help=dra;
                    dra=sou;
                    sou=help;
                }
                if(tempRoot->nextPort[i]->expression!="%")
                    tempHead->nextTran[i]=redoNmosNet(tempRoot->nextPort[i], tempHead, dra, sou);
            }
        }
        
    }
    else//it is a port, quit the recursion and name the transistor
    {
        newNet->tran=tempRoot->expression;
        newNet->num=tempRoot->Number;
        newNet->mos='N';
        newNet->drain=dra;
        newNet->source=sou;
        if(newNet->tran!="%")//do not show the pseudo line
        {
            for(cont=0;cont<useContE-1;cont++)
            {
                if(newNet->drain==useList[cont]&&newNet->drain!=0)
                {
                    newNet->drain=cont+2;
                }
                if(newNet->source==useList[cont]&&newNet->source!=0)
                {
                    newNet->source=cont+2;
                }
            }
			if(newNet->drain==100 && newNet->source==0)
			{
				vector<string> temp;
				temp.push_back(newNet->tran);
				temp.push_back("OUT");
				temp.push_back("GND");
				NnetOrder.push_back(temp);
			}
            else if(newNet->drain==0 && newNet->source==100)
			{
				vector<string> temp;
				temp.push_back(newNet->tran);
				temp.push_back("GND");
				temp.push_back("OUT");
				NnetOrder.push_back(temp);
			}
            else if(newNet->drain==100)
			{
				string str;
				str.push_back(newNet->mos);
				ostringstream ostr;
				ostr<<newNet->source;
				string s=ostr.str();
				str += s;
				vector<string> temp;
				temp.push_back(newNet->tran);
				temp.push_back("OUT");
				temp.push_back(str);
				NnetOrder.push_back(temp);
			}
			else if(newNet->drain==0)
			{
				string str;
				str.push_back(newNet->mos);
				ostringstream ostr;
				ostr<<newNet->source;
				string s=ostr.str();
				str += s;
				vector<string> temp;
				temp.push_back(newNet->tran);
				temp.push_back("GND");
				temp.push_back(str);
				NnetOrder.push_back(temp);
			}
            else if(newNet->source==0)
			{
				string str;
				str.push_back(newNet->mos);
				ostringstream ostr;
				ostr<<newNet->drain;
				string s=ostr.str();
				str += s;
				vector<string> temp;
				temp.push_back(newNet->tran);
				temp.push_back(str);
				temp.push_back("GND");
				NnetOrder.push_back(temp);
			}
			else if(newNet->source==100)
			{
				string str;
				str.push_back(newNet->mos);
				ostringstream ostr;
				ostr<<newNet->drain;
				string s=ostr.str();
				str += s;
				vector<string> temp;
				temp.push_back(newNet->tran);
				temp.push_back(str);
				temp.push_back("OUT");
				NnetOrder.push_back(temp);
			}
			else
			{
				string str1;
				str1.push_back(newNet->mos);
				ostringstream ostr1;
				ostr1<<newNet->drain;
				string s1=ostr1.str();
				str1 += s1;
				string str2;
				str2.push_back(newNet->mos);
				ostringstream ostr2;
				ostr2<<newNet->source;
				string s2=ostr2.str();
				str2 += s2;
				vector<string> temp;
				temp.push_back(newNet->tran);
				temp.push_back(str1);
				temp.push_back(str2);
				NnetOrder.push_back(temp);
			}
            if(newNet->drain<newNet->source)//disregard the eular path
            {
                help=newNet->drain;
                newNet->drain=newNet->source;
                newNet->source=help;
            }
            if(newNet->drain==100 && newNet->source==0)
			{
				vector<string> temp;
				temp.push_back(newNet->tran);
				temp.push_back("OUT");
				temp.push_back("GND");
				Nnet.push_back(temp);
                cout<<newNet->tran<<"\t"<<"OUT"<<"\t"<<"GND"<<endl;
			}
            else if(newNet->drain==0 && newNet->source==100)
			{
				vector<string> temp;
				temp.push_back(newNet->tran);
				temp.push_back("GND");
				temp.push_back("OUT");
				Nnet.push_back(temp);
                cout<<newNet->tran<<"\t"<<"GND"<<"\t"<<"OUT"<<endl;
			}
            else if(newNet->drain==100)
			{
				string str;
				str.push_back(newNet->mos);
				ostringstream ostr;
				ostr<<newNet->source;
				string s=ostr.str();
				str += s;
				vector<string> temp;
				temp.push_back(newNet->tran);
				temp.push_back("OUT");
				temp.push_back(str);
				Nnet.push_back(temp);
                cout<<newNet->tran<<"\t"<<"OUT"<<"\t"<<newNet->mos<<newNet->source<<endl;
			}
			else if(newNet->drain==0)
			{
				string str;
				str.push_back(newNet->mos);
				ostringstream ostr;
				ostr<<newNet->source;
				string s=ostr.str();
				str += s;
				vector<string> temp;
				temp.push_back(newNet->tran);
				temp.push_back("GND");
				temp.push_back(str);
				Nnet.push_back(temp);
                cout<<newNet->tran<<"\t"<<"GND"<<"\t"<<newNet->mos<<newNet->source<<endl;
			}
            else if(newNet->source==0)
			{
				string str;
				str.push_back(newNet->mos);
				ostringstream ostr;
				ostr<<newNet->drain;
				string s=ostr.str();
				str += s;
				vector<string> temp;
				temp.push_back(newNet->tran);
				temp.push_back(str);
				temp.push_back("GND");
				Nnet.push_back(temp);
                cout<<newNet->tran<<"\t"<<newNet->mos<<newNet->drain<<"\t"<<"GND"<<"\t"<<endl;
			}
			else if(newNet->source==100)
			{
				string str;
				str.push_back(newNet->mos);
				ostringstream ostr;
				ostr<<newNet->drain;
				string s=ostr.str();
				str += s;
				vector<string> temp;
				temp.push_back(newNet->tran);
				temp.push_back(str);
				temp.push_back("OUT");
				Nnet.push_back(temp);
                cout<<newNet->tran<<"\t"<<newNet->mos<<newNet->drain<<"\t"<<"OUT"<<endl;
			}
			else
			{
				string str1;
				str1.push_back(newNet->mos);
				ostringstream ostr1;
				ostr1<<newNet->drain;
				string s1=ostr1.str();
				str1 += s1;
				string str2;
				str2.push_back(newNet->mos);
				ostringstream ostr2;
				ostr2<<newNet->source;
				string s2=ostr2.str();
				str2 += s2;
				vector<string> temp;
				temp.push_back(newNet->tran);
				temp.push_back(str1);
				temp.push_back(str2);
				Nnet.push_back(temp);
                cout<<newNet->tran<<"\t"<<newNet->mos<<newNet->drain<<"\t"<<newNet->mos<<newNet->source<<endl;
			}
		}
    }
    return tempHead;
}
/////////////////////////////////////
net *booExpression::createPmosNet(gate *tempRoot, net *tempHead, int dra, int sou)//p-mos
{
    int i;
    int h;
    int newNodes[MAX];
    int help=0;
    net *newNet=new net;//create a new net
    if(tempHead==NULL)//create the original drain and source
    {
        newNet->drain=OUTT;
        newNet->source=GND;
        newNet->tran=tempRoot->expression;
        newNet->num=tempRoot->Number;
        newNet->mos='P';
        tempHead=newNet;//after creating the original diagram
        Phead=newNet;
    }
    if(tempRoot->Number!=0)//there's child lines
    {
        if(tempRoot->Symbol=='+')//for p-mos, it is serial
        {
            newNodes[0]=dra;//create new nodes
            newNodes[tempRoot->realNumber]=sou;
            //cout<<tempRoot->realNumber<<endl;
            //cout<<newNodes[0]<<endl;
            for(i=1;i<tempRoot->realNumber;i++)
            {
                newNodes[i]=dra+(sou-dra)/tempRoot->realNumber*i;//create new nodes
                if(!(newNodes[i]%2))
                    newNodes[i]=newNodes[i]+1;
                newNodes[i]=changeNum(newNodes[i]);
                //cout<<newNodes[i]<<endl;
            }
            //cout<<newNodes[tempRoot->realNumber]<<endl;
            for(i=0;i<tempRoot->Number;i++)
            {
                if(tempRoot->nextPort[i]->expression=="%")
                {
                    for(h=tempRoot->Number;h>0;h--)
                        newNodes[h]=newNodes[h-1];
                }
                else
                    tempHead->nextTran[i]=createPmosNet(tempRoot->nextPort[i], tempHead, newNodes[i], newNodes[i+1]);
            }
        }
        else//for p-mos, it is parellel
        {
            for(i=0;i<tempRoot->Number;i++)
            {
                if(i>0)//afte the first line, each later lines should all do reverse
                {
                    help=dra;
                    dra=sou;
                    sou=help;
                }
                if(tempRoot->nextPort[i]->expression!="%")
                    tempHead->nextTran[i]=createPmosNet(tempRoot->nextPort[i], tempHead, dra, sou);
            }
        }
        
    }
    else//it is a port, quit the recursion and name the transistor
    {
        newNet->tran=tempRoot->expression;
        newNet->num=tempRoot->Number;
        newNet->mos='P';
        newNet->drain=dra;
        newNet->source=sou;
        //if(newNet->tran!="%")//do not show the pseudo line
        //    cout<<newNet->tran<<"\t"<<newNet->drain<<"\t"<<newNet->source<<endl;
    }
    
    return tempHead;
    
}
/////////////////////////////////////
net *booExpression::createNmosNet(gate *tempRoot, net *tempHead, int dra, int sou)//n-mos
{
    int i;
    int h;
    int newNodes[MAX];
    int help=0;
    net *newNet=new net;//create a new net
    if(tempHead==NULL)//create the original drain and source
    {
        newNet->drain=OUTT;
        newNet->source=GND;
        newNet->tran=tempRoot->expression;
        newNet->num=tempRoot->Number;
        newNet->mos='N';
        tempHead=newNet;//after creating the original diagram
        Nhead=newNet;
    }
    if(tempRoot->Number!=0)//there's child lines
    {
        if(tempRoot->Symbol=='*')//for n-mos, it is serial
        {
            newNodes[0]=dra;//create new nodes
            //cout<<"dra "<<dra<<endl;
            newNodes[tempRoot->realNumber]=sou;
            //cout<<"sou "<<sou<<endl;
            //cout<<tempRoot->realNumber<<endl;
            for(i=1;i<tempRoot->realNumber;i++)
            {
                newNodes[i]=dra+(sou-dra)/tempRoot->realNumber*i;//create new nodes
                //cout<<newNodes[i]<<endl;
                if(newNodes[i]%2)
                {
                    newNodes[i]=newNodes[i]+1; 
                }
                newNodes[i]=changeNum(newNodes[i]);
                //cout<<newNodes[i]<<endl;
            }
            for(i=0;i<tempRoot->Number;i++)
            {
                if(tempRoot->nextPort[i]->expression=="%")
                {
                    for(h=tempRoot->Number;h>0;h--)
                        newNodes[h]=newNodes[h-1];
                }
                else
                    tempHead->nextTran[i]=createNmosNet(tempRoot->nextPort[i], tempHead, newNodes[i], newNodes[i+1]);
            }
        }
        else//for n-mos, it is parellel
        {
            for(i=0;i<tempRoot->Number;i++)
            {
                if(i>0)//afte the first line, each later lines should all do reverse
                {
                    help=dra;
                    dra=sou;
                    sou=help;
                }
                if(tempRoot->nextPort[i]->expression!="%")
                    tempHead->nextTran[i]=createNmosNet(tempRoot->nextPort[i], tempHead, dra, sou);
            }
        }
        
    }
    else//it is a port, quit the recursion and name the transistor
    {
        newNet->tran=tempRoot->expression;
        newNet->num=tempRoot->Number;
        newNet->mos='N';
        newNet->drain=dra;
        newNet->source=sou;
        //if(newNet->tran!="%")//do not show the pseudo line
        //    cout<<newNet->tran<<"\t"<<newNet->drain<<"\t"<<newNet->source<<endl;
    }
    
    return tempHead;
    
}
////////////////////////////////////////
gate *booExpression::initGateTree(string s)//back the type of the Gate, gate or port
{
    int num=0;
    checkBooExpression(s);
    string b[MAX];
    string pseudo;
    vector<vector<char> >temp(MAX,vector<char>(' '));
	char symbol[MAX];
    string gateColor;
    int i=0;
    int cont;
    int g;//number of the new combined gates
    int l;
    int portCon=0;
    int getChild=0;
	int len;
	len=s.length();
    gate *tempRoot;//the pointer of the new gate
    if(s.length()==1)//find a port
    {
        gateType='p';
        port=s[0];
        if(s=="%")
            gateColor="ww";//pseudo
        else
            gateColor="bb";//valid
        tempRoot=createGate(s,num,gateSymbol,gateType,gateColor,i);//create a new Gate
        return tempRoot;
    }
    else//find a gate
    {
        for(i=0,g=0,cont=0,getChild=0;i<len;i++)
        {
            if(s[i]=='(')//exclude the parent boolean expression
            {
                getChild++;
            }
            if(getChild>1)//finding a second '(', means a start of a child boolean expression, find a new gate
            {
				temp[g].resize(len);
				//cout<<g<<" "<<cont<<" "<<i<<endl;
                temp[g][cont]=s[i];//start storing the child gate
                cont++;
                if(s[i]==')')
                {
                    getChild--;
                    if(getChild==1)//find a complete child gate
                    {
                        b[g].resize(cont);
                        for(l=0;l<cont;l++)
                            b[g][l]=temp[g][l];//generate a child
                        g++;
                        cont=0;
                    }
                }
            }
            else if((s[i]>='a'&&s[i]<='z')||(s[i]>='A'&&s[i]<='Z'))//find a single input
            {
                b[g].resize(1);
                b[g][0]=s[i];
                g++;
            }
            else if(s[i]!='('&&s[i]!=')'&&s[i]!=' '&&s[i]!='~')//find the gate symbol
            {
                symbol[portCon]=s[i];//initialize the symbol
                portCon++;
                if(symbol[0]!=s[i])//find diffentt symbol
                {
                    legal=false;//different symbol
                    //quitControl();//restart input
                }
            }
        }
        gateType='g';
        gateSymbol=symbol[0];
        num=g;
    }
    if(num%2)//odd
    {
        num=num;
        gateColor="bb";//all black
        tempRoot=createGate(s,num,gateSymbol,gateType,gateColor,i);//create a new Gate
        for(i=0;i<num;i++)//find the ports and gates under the new Gate
            tempRoot->nextPort[i]=initGateTree(b[i]);
    }
    else//even
    {
        num=num+1;//add a pseudo line
        gateColor="bw";//there's a pseudo line
        tempRoot=createSpGate(s,num,gateSymbol,gateType,gateColor,i);//create a new Gate
        for(i=0;i<num-1;i++)//find the ports and gates under the new Gate
            tempRoot->nextPort[i]=initGateTree(b[i]);
        pseudo="%";
        tempRoot->nextPort[num-1]=initGateTree(pseudo);//add a pseudo port
    }
    return tempRoot;
}
/////////////////////////////////////////////////////////
gate *booExpression::createGate(string s, int n, char sym, char type, string col, int i)//
{
    int j;
    //
    gate *newGate=new gate;//create a new gate
    newGate->expression=s;
    newGate->realNumber=n;
    newGate->color=col;
    newGate->Number=n;
    if(type=='g')
        newGate->Symbol=sym;
    else
        newGate->Symbol=' ';
    newGate->gorp=type;
    for(j=0;j<newGate->Number;j++)
        newGate->nextPort[j]=NULL;
    //
    if(root==NULL)
    {
        root=newGate;
        return root;
    }
    else
    {
        return newGate;
    }
}
/////////////////////////////////////////////////////////
gate *booExpression::createSpGate(string s, int n, char sym, char type, string col, int i)//
{
    int j;
    //
    gate *newGate=new gate;//create a new gate
    newGate->expression=s;
    newGate->realNumber=n-1;
    newGate->color=col;
    newGate->Number=n;
    if(type=='g')
        newGate->Symbol=sym;
    else
        newGate->Symbol=' ';
    newGate->gorp=type;
    for(j=0;j<newGate->Number;j++)
        newGate->nextPort[j]=NULL;
    //
    if(root==NULL)
    {
        root=newGate;
        return root;
    }
    else
    {
        gate *current;
        current=newGate;
        return current;
    }
}
/////////////////////////////////////////
gate *booExpression::reshape(gate *temp)
{
    int i;
    int j=0;
    int wwcont=0;
    int wbcont=0;
    int bbcont=0;
    int bwcont=0;
    gate *checkChild;
    gate *help;

    if(temp!=NULL && temp->expression.length()!=1)//if it is a valid gate
    {
        for(i=0;i<temp->Number;i++)//reshape the child gates
            reshape(temp->nextPort[i]);
        for(i=0;i<temp->Number;i++)
        {
            if(temp->nextPort[i]->color=="ww")//cont the number of the "ww"
                wwcont++;
            if(temp->nextPort[i]->color=="wb")//cont the number of the "wb"
                wbcont++;
            if(temp->nextPort[i]->color=="bb")//cont the number of the "bb"
                bbcont++;
            if(temp->nextPort[i]->color=="bw")//cont the number of the "bw"
                bwcont++;
        }
        while(wbcont<bwcont)//"wb" should not be less than "bw"
        {
            for(i=j;i<temp->Number;i++)//check all the gates
            {
                if(temp->nextPort[i]->color=="bw")
                {
                    temp->nextPort[i]=reverse(temp->nextPort[i]);//find a "bw" and reverse it
                    break;
                }
            }
            for(wbcont=bwcont=0,i=0;i<temp->Number;i++)//calculate again
            {
                if(temp->nextPort[i]->color=="wb")//cont the number of the "wb"
                    wbcont++;
                if(temp->nextPort[i]->color=="bw")//cont the number of the "bw"
                    bwcont++;
            }
        }
        while((wbcont-bwcont)>1)//"wb" should not 2 more than "bw"
        {
            for(i=j;i<temp->Number;i++)//check all the gates
            {
                if(temp->nextPort[i]->color=="wb")
                {
                    temp->nextPort[i]=reverse(temp->nextPort[i]);//find a "bw" and reverse it
                    break;
                }
            }
            for(wbcont=bwcont=0,i=0;i<temp->Number;i++)//calculate again
            {
                if(temp->nextPort[i]->color=="wb")//cont the number of the "wb"
                    wbcont++;
                if(temp->nextPort[i]->color=="bw")//cont the number of the "bw"
                    bwcont++;
            }
        }
        for(i=j;i<temp->Number;i++)
        {
            if(temp->nextPort[i]->color=="ww")//move all the "ww" to the top
            {
                help=temp->nextPort[j];
                temp->nextPort[j]=temp->nextPort[i];
                temp->nextPort[i]=help;//change place
                j++;
            }
        }
        for(i=j;i<temp->Number;i++)
        {
            if(temp->nextPort[i]->color=="wb")//move one "wb" next to "ww"
            {
                help=temp->nextPort[j];
                temp->nextPort[j]=temp->nextPort[i];
                temp->nextPort[i]=help;//change place
                j++;
                break;
            }
        }
        for(i=j;i<temp->Number;i++)
        {
            if(temp->nextPort[i]->color=="bb")//move all the "bb" next to "wb"
            {
                help=temp->nextPort[j];
                temp->nextPort[j]=temp->nextPort[i];
                temp->nextPort[i]=help;//change place
                j++;
            }
        }
        while(j<temp->Number)//all set
        {
            checkChild=temp->nextPort[j-1];
            if(checkChild->color=="bb"||checkChild->color=="wb")//the last child is "bb" or "wb"
            {
                for(i=j;i<temp->Number;i++)
                {
                    if(temp->nextPort[i]->color=="bw")//move one "bw" next to "bb" or "wb"
                    {
                        help=temp->nextPort[j];
                        temp->nextPort[j]=temp->nextPort[i];
                        temp->nextPort[i]=help;//change place
                        j++;
                        break;
                    }
                }
            }
            else//the last child is "ww" or "bw"
            {
                for(i=j;i<temp->Number;i++)
                {
                    if(temp->nextPort[i]->color=="wb")//move one "wb" next to "bw"
                    {
                        help=temp->nextPort[j];
                        temp->nextPort[j]=temp->nextPort[i];
                        temp->nextPort[i]=help;//change place
                        j++;
                        break;
                    }
                }
            }
        }
        temp->color=changeColor(temp);//change the gate color
    }
	return temp;
}
////////////////////////////////////////
gate *booExpression::reverse(gate *temp)
{
    int i,j;//record the number of the lines
    gate *t[MAX];
    if(temp!=NULL && temp->expression.length()!=1)//if it is a valid gate
    {
        for(i=0;i<temp->Number;i++)
            temp->nextPort[i]=reverse(temp->nextPort[i]);//reverse the child gates
        for(i=0,j=temp->Number-1;j>=0;j--,i++)//reverse the sequence of the gate's inputs
            t[i]=temp->nextPort[j];
        for(i=0;i<temp->Number;i++)
            temp->nextPort[i]=t[i]; 
        temp->color=changeColor(temp);//change the gate color
        return temp;
    }
    return temp;
}
////////////////////////////////////////////
string booExpression::changeColor(gate *temp)
{
    if(temp->nextPort[0]->color[0]=='w'&&temp->nextPort[temp->Number-1]->color[1]=='w')
        return "ww";
    else if(temp->nextPort[0]->color[0]=='w'&&temp->nextPort[temp->Number-1]->color[1]=='b')
        return "wb";
    else if(temp->nextPort[0]->color[0]=='b'&&temp->nextPort[temp->Number-1]->color[1]=='b')
        return "bb";
	else //if(temp->nextPort[0]->color[0]=='b'&&temp->nextPort[temp->Number-1]->color[1]=='w')
        return "bw";
}
/////////////////////////////////////
int booExpression::changeNum(int n)
{
    int j;
    if(n%2&&n>1)//odd
    {
        for(j=1;j<useContO;j=j+2)//check all the number used, rename a unique one
            if(n==useList[j])
                n=n+2;
        useList[useContO]=n;//add it to the uselist
        useContO=useContO+2;
    }
    else if(n%2==0&&n>0)//even
    {
        for(j=0;j<useContE;j=j+2)//check all the number used, rename a unique one
            if(n==useList[j])
                n=n+2;
        useList[useContE]=n;//add it to the uselist
        useContE=useContE+2;
    }
    return n;
}
/////////////////////////////////////
int booExpression::changeNum2(int n)
{
    int j;
    if(n%2&&n>1)//odd
    {
        for(j=1;j<useContO2;j=j+2)//check all the number used, rename a unique one
            if(n==useList2[j])
                n=n+2;
        useList2[useContO2]=n;//add it to the uselist
        useContO2=useContO2+2;
    }
    else if(n%2==0&&n>0)//even
    {
        for(j=0;j<useContE2;j=j+2)//check all the number used, rename a unique one
            if(n==useList2[j])
                n=n+2;
        useList2[useContE2]=n;//add it to the uselist
        useContE2=useContE2+2;
    }
    return n;
}
/////////////////////////////////////
void booExpression::redoUseList()
{
    int i,j;
    int temp;
    for(i=1;i<useContO-1;i=i+2)
    {
        for(j=1;j<useContO-2-i;j=j+2)
        {
            if(useList[j]>useList[j+2])
            {
                temp=useList[j];
                useList[j]=useList[j+2];
                useList[j+2]=temp;
            }
        }
    }
    //for(i=1;i<useContO;i=i+2)
    //    cout<<useList[i]<<" "<<i<<endl;
    //cout<<useContE<<endl;
    for(i=0;i<useContE-1;i=i+2)
    {
        for(j=0;j<useContE-2-i;j=j+2)
        {
            if(useList[j]>useList[j+2])
            {
                temp=useList[j];
                useList[j]=useList[j+2];
                useList[j+2]=temp;
            }
        }
    }
    //for(i=0;i<useContE;i=i+2)
     //   cout<<useList[i]<<" "<<i<<endl;
}
//////////////////////////////////////////
void booExpression::inputControl()
{
    string s;
    cout<<"input your boolean expression ( please use the brackets to represent a gate) :"<<endl;
    cout<<"OUT = ";
    while(getline(cin,s))
    {
        checkBooExpression(s);//check the input
        if(!legal)
        {
            cout<<"illegal input! please try again! please check your expression and input again!"<<endl;
            cout<<"OUT = ";
        }
        else
            break;
    }
    boolean[0]=s;
}
///////////////////////////////////////////quit
void booExpression::quitControl()
{
    if(!legal)
    {
        cout<<"illegal input! please try again!"<<endl;
        //inputControl();
    }
}
///////////////make sure the input is legal and calculate the number of the gates in the boolean expression
void booExpression::checkBooExpression(string s)
{
    int length=s.length();
    int i;
    int lb=0,rb=0;
    for(i=0;i<length;i++)
    {
        if((s[i]>='a'&&s[i]<='z')||(s[i]>='A'&&s[i]<='Z')||s[i]=='*'||s[i]=='+'||s[i]==' '||s[i]=='~'||s[i]=='('||s[i]==')')
        {
            if(s[i]=='(')
            {
                lb++;
            }
            if(s[i]==')')
            {
                rb++;
            }
        }
        else//illegal
        {
            legal=false;
        }
    }
    if(lb!=rb)//the left brackets and the right brackets are not equal
        legal=false;
    else
    {
        legal=true;
    }
}
//////////////////////////////////////////
void booExpression::displayBoolExpression()
{
    cout<<endl;
    cout<<"Your input bool expression is : "<<endl;
    cout<<"OUT = "<<boolean[0]<<endl;
    cout<<endl;
}
////////////////////////////////////////
gate *booExpression::inspectGateTree(gate *temp)
{
    int i;
    if(temp!=NULL)
    {
        if(temp->gorp=='p')//only display ports
        {
            //cout<<"the gate info is :"<<temp->expression<<"  "<<temp->Symbol<<"  "<<temp->realNumber<<" "<<temp->Number<<"  "<<temp->color<<" "<<temp->gorp<<endl;
            //cout<<temp->expression<<" ";
            polySilicon[polyCont]=temp->expression[0];//store the polysilicon info
            polyCont++;
        }
        for(i=0;i<temp->Number;i++)
            inspectGateTree(temp->nextPort[i]);
    }
	return temp;
}
/////////////////////////////////////
void booExpression::displayPolySilicon(void)
{
    int i,j;
    inspectGateTree(root);
    cout<<"The polySilicon gate sequence is :"<<endl;
    for(i=1;i<polyCont;)//combine the % in a row
    {
        if(polySilicon[i]==polySilicon[i-1])
        {
            for(j=i-1;j<polyCont;j++)
                polySilicon[j]=polySilicon[j+1];
            polyCont--;
        }
        else
            i++;
    }
    if(polySilicon[0]=='%')
    {
        for(j=0;j<polyCont;j++)
            polySilicon[j]=polySilicon[j+1];
        polyCont--;
    }
    if(polySilicon[polyCont-1]=='%')
        polyCont--;
    for(i=0;i<polyCont;i++)
        cout<<polySilicon[i]<<" ";
    cout<<endl;
}
////////////////////////////////////
