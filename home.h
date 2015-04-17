#pragma once

#include <msclr\marshal_cppstd.h>
#include <iostream>
#include <string>
#include "layout.h"


using namespace std;

namespace AutoLayout {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// MyForm 摘要
	/// </summary>
	public ref class MyForm : public System::Windows::Forms::Form
	{
	public:
		MyForm(void)
		{
			InitializeComponent();
			//
			//TODO: 在此处添加构造函数代码
			//
		}
		booExpression *exp;

	protected:
		/// <summary>
		/// 清理所有正在使用的资源。
		/// </summary>
		~MyForm()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::TextBox^  textBoxInpt;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::Label^  label2;
	public: System::Windows::Forms::TextBox^  textBoxPoly;
	private: System::Windows::Forms::TextBox^  textBoxPnet;
	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::Label^  label4;
	private: System::Windows::Forms::TextBox^  textBoxNnet;
	private: System::Windows::Forms::Button^  button1;
	private: System::Windows::Forms::Label^  label5;
	private: System::Windows::Forms::Label^  label6;



	private: System::Void button1_Click(System::Object^  sender, System::EventArgs^  e);
	private: Void MyForm::FillRectangleRectangle(float a, float b, float c, float d);//System::Drawing::Rectangle^ rect)
	private: Void MyForm::DrawRectangleRectangle(float a, float b, float c, float d);//System::Drawing::Rectangle^ rect)
	
	protected: 


	private:
		/// <summary>
		/// 必需的设计器变量。
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// 设计器支持所需的方法 - 不要
		/// 使用代码编辑器修改此方法的内容。
		/// </summary>
		void InitializeComponent(void)
		{
			this->button1 = (gcnew System::Windows::Forms::Button());
			this->textBoxInpt = (gcnew System::Windows::Forms::TextBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->textBoxPoly = (gcnew System::Windows::Forms::TextBox());
			this->textBoxPnet = (gcnew System::Windows::Forms::TextBox());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->textBoxNnet = (gcnew System::Windows::Forms::TextBox());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->label6 = (gcnew System::Windows::Forms::Label());
			this->SuspendLayout();
			// 
			// button1
			// 
			this->button1->Location = System::Drawing::Point(530, 38);
			this->button1->Margin = System::Windows::Forms::Padding(10);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(75, 23);
			this->button1->TabIndex = 0;
			this->button1->Text = L"submit";
			this->button1->UseVisualStyleBackColor = true;
			this->button1->Click += gcnew System::EventHandler(this, &MyForm::button1_Click);
			// 
			// textBoxInpt
			// 
			this->textBoxInpt->Location = System::Drawing::Point(61, 40);
			this->textBoxInpt->Margin = System::Windows::Forms::Padding(10);
			this->textBoxInpt->Name = L"textBoxInpt";
			this->textBoxInpt->Size = System::Drawing::Size(451, 21);
			this->textBoxInpt->TabIndex = 1;
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(19, 19);
			this->label1->Margin = System::Windows::Forms::Padding(10);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(179, 12);
			this->label1->TabIndex = 2;
			this->label1->Text = L"Input your boolean expression";
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(19, 71);
			this->label2->Margin = System::Windows::Forms::Padding(10);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(197, 12);
			this->label2->TabIndex = 3;
			this->label2->Text = L"The optimal polysilicon sequence";
			// 
			// textBoxPoly
			// 
			this->textBoxPoly->Location = System::Drawing::Point(21, 90);
			this->textBoxPoly->Margin = System::Windows::Forms::Padding(10);
			this->textBoxPoly->Name = L"textBoxPoly";
			this->textBoxPoly->Size = System::Drawing::Size(584, 21);
			this->textBoxPoly->TabIndex = 4;
			// 
			// textBoxPnet
			// 
			this->textBoxPnet->AcceptsReturn = true;
			this->textBoxPnet->AcceptsTab = true;
			this->textBoxPnet->Location = System::Drawing::Point(21, 144);
			this->textBoxPnet->Multiline = true;
			this->textBoxPnet->ScrollBars =  ScrollBars::Vertical;
			this->textBoxPnet->Name = L"textBoxPnet";
			this->textBoxPnet->Size = System::Drawing::Size(260, 262);
			this->textBoxPnet->TabIndex = 5;
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(19, 129);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(77, 12);
			this->label3->TabIndex = 6;
			this->label3->Text = L"Pmos Netlist";
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Location = System::Drawing::Point(343, 129);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(77, 12);
			this->label4->TabIndex = 7;
			this->label4->Text = L"Nmos Netlist";
			// 
			// textBoxNnet
			// 
			this->textBoxNnet->AcceptsReturn = true;
			this->textBoxNnet->AcceptsTab = true;
			this->textBoxNnet->Location = System::Drawing::Point(345, 144);
			this->textBoxNnet->Multiline = true;
			this->textBoxNnet->ScrollBars =  ScrollBars::Vertical;
			this->textBoxNnet->Name = L"textBoxNnet";
			this->textBoxNnet->Size = System::Drawing::Size(260, 262);
			this->textBoxNnet->TabIndex = 8;
			// 
			// label5
			// 
			this->label5->AutoSize = true;
			this->label5->Location = System::Drawing::Point(20, 43);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(41, 12);
			this->label5->TabIndex = 10;
			this->label5->Text = L"OUT = ";
			// 
			// label6
			// 
			this->label6->AutoSize = true;
			this->label6->Location = System::Drawing::Point(384, 420);
			this->label6->Name = L"label6";
			this->label6->Size = System::Drawing::Size(221, 12);
			this->label6->TabIndex = 11;
			this->label6->Text = L"Designed by Zhenshen Qiu, 12-15-2013";
			// 
			// MyForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(624, 441);
			this->Controls->Add(this->label6);
			this->Controls->Add(this->label5);
			this->Controls->Add(this->label4);
			this->Controls->Add(this->label3);
			this->Controls->Add(this->textBoxPnet);
			this->Controls->Add(this->textBoxNnet);
			this->Controls->Add(this->textBoxPoly);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->textBoxInpt);
			this->Controls->Add(this->button1);
			this->MaximumSize = System::Drawing::Size(640, 480);
			this->MinimumSize = System::Drawing::Size(640, 480);
			this->Name = L"MyForm";
			this->Text = L"Auto Layout Tool v4.0";
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion


};
}
