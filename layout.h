//
//  gate.h
//  
//
//  Created by Zhenshen Qiu on 9/7/13.
//
//

#ifndef ____layout__
#define ____layout__

#include <iostream>
#include <stdlib.h>
#include <sstream>
#include <cstdlib>
#include <string>
#include <vector>
using namespace std;

#define MAX 1000//larger value permits larger inputs
#define VDD 1//source of p-mos
#define GND 0//source of n-mos
#define OUTT 100//drain
///////////////////////////////construct a gateTree to store the gate information from the boolean expression
typedef struct gate
{
    string expression;//the gate's boolean expression
    char Symbol;//gate type: 'and' or 'or'
    int realNumber;//the real input number of a gate
    int Number;//input number
    char gorp;//struct type: gate or port;
    string color;//only 4 color: ww,wb,bb,bw
    gate *nextPort[MAX];
}GATE;
////////////////////////////////
typedef struct net//build a netlist
{
    string tran;//the transister's name
    string toDrain;//whether connected to  drain
    string toSource;//whether connected to source
    char mos;//p-mos or n-mos
    int num;//childs number
    int source;//input nod number
    int drain;//output node number
    net *nextTran[MAX];
}NET;
///////////////////////////////////////the boolean expression class
class booExpression
{
public:
    booExpression();//initialize the boolean expression, store it if legal, reject it otherwise
	booExpression(string);//initialize the boolean expression, store it if legal, reject it otherwise
    void inputControl(void);
    void quitControl(void);
    void setBooExpression(string s);
    int inputBooExpression(void);
    void checkBooExpression(string s);
    gate *initGateTree(string s);
    gate *createGate(string s, int n, char sym, char type, string col, int i);
    gate *createSpGate(string s, int n, char sym, char type, string col, int i);
    gate *inspectGateTree(gate *temp);
    void displayBoolExpression(void);
    void displayPolySilicon(void);
    gate *reshape(gate *temp);
    gate *reverse(gate *temp);
    int changeNum(int n);
    int changeNum2(int n);
    void redoUseList(void);
    string changeColor(gate *temp);
    net *createPmosNet(gate *tempRoot, net *tempHead, int dra, int sou);
    net *redoPmosNet(gate *tempRoot, net *tempHead, int dra, int sou);
    net *createNmosNet(gate *tempRoot, net *tempHead, int dra, int sou);
    net *redoNmosNet(gate *tempRoot, net *tempHead, int dra, int sou);
	void generate(void);
    /////////////////////////////////////////////////
    gate *root;
    net *Phead;
    net *Nhead;
    int useList[MAX];
    int useContE;
    int useContO;
    int useList2[MAX];
    int useContE2;
    int useContO2;
    ////////////////////////////
	char polySilicon[MAX];
	vector<vector<string> > Pnet;
	vector<vector<string> > Nnet;
	vector<vector<string> > PnetOrder;
	vector<vector<string> > NnetOrder;
private:
    string boolean[MAX];//boolean expression
    int polyCont;
    int gateNum;
	int expLength;
    char gateSymbol;
    char gateType;//gorp
    char port;
    bool legal;
};

#endif /* defined(____layout__) */
