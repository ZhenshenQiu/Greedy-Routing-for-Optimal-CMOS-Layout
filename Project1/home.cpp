#include "home.h"
#include "autoLayout.h"

using namespace AutoLayout;


Void MyForm::button1_Click(System::Object^  sender, System::EventArgs^  e) 
{
	if(String::IsNullOrWhiteSpace(textBoxInpt->Text))
	{
		MessageBox::Show(this, "Please input the expression first!");
		textBoxInpt->Focus();
	}
	else
	{
		booExpression expression(msclr::interop::marshal_as< std::string >(textBoxInpt->Text));
		expression.generate();
		exp = new booExpression;
		*exp = expression;
		unsigned int cont = 0;
		for(int i=0; i<100; i++)
		{
			if(expression.polySilicon[i] == '%')
				cont++;
		}
		if(expression.polySilicon[expression.Pnet.size() + cont - 1] == '%')
			cont--;
		textBoxPoly->Clear();
		textBoxPoly->Text = System::Runtime::InteropServices::Marshal::PtrToStringAnsi((IntPtr)(expression.polySilicon));
		textBoxPoly->Text = textBoxPoly->Text->Substring(0, expression.Pnet.size() + cont);
		String ^showP = "Gate\t" + "Drain\t" + "Source";
		for(unsigned int i=0; i<expression.Pnet.size(); i++)
		{
			showP += System::Environment::NewLine;
			for(unsigned int j=0; j<expression.Pnet[i].size(); j++)
			{
				String ^tmp = gcnew String(expression.Pnet[i][j].c_str());
				showP += tmp + "\t";
			}
		}
		textBoxPnet->Text = showP;
		String ^showN = "Gate\t" + "Drain\t" + "Source";
		for(unsigned int i=0; i<expression.Nnet.size(); i++)
		{
			showN += System::Environment::NewLine;
			for(unsigned int j=0; j<expression.Nnet[i].size(); j++)
			{
				String ^tmp = gcnew String(expression.Nnet[i][j].c_str());
				showN += tmp + "\t";
			}
		}
		textBoxNnet->Text = showN;
		if ((MessageBox::Show(this, "To get an autoLayout?","reminder", MessageBoxButtons::YesNo ) == ::DialogResult::Yes))
		{
			autoLayout ^f1 = gcnew autoLayout(this);
			f1->Owner = this;
			//f1->pictureBox1->Image = System::Drawing::Image::FromFile("1.png");
			f1->Show(this);
		}
	}
	
}

/**********************   Draw the module to the region   ***************************/
Void MyForm::FillRectangleRectangle(float a, float b, float c, float d)//System::Drawing::Rectangle^ rect)
{
	int ran = rand() % 5 + 1;
	SolidBrush^ Brush = gcnew SolidBrush(Color::GreenYellow);;
	Graphics^ formGraphics;
	formGraphics = this->CreateGraphics();
	formGraphics->FillRectangle(Brush, a, b, c, d);
	delete Brush;
	delete formGraphics;
	DrawRectangleRectangle(a , b, c, d);
}

/**********************   Draw the module to the region   ***************************/
Void MyForm::DrawRectangleRectangle(float a, float b, float c, float d)//System::Drawing::Rectangle^ rect)
{
	Pen^ pen = gcnew Pen(Color::Black);
	Graphics^ formGraphics;
	formGraphics = this->CreateGraphics();
	formGraphics->DrawRectangle(pen, a, b, c, d);
	delete pen;
	delete formGraphics;
}