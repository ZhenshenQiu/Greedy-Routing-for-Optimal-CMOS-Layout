#pragma once
#include "home.h"
#include "material.h"
#include "column.h"
#include "path.h"
#include "track.h"
#include <stack>
#include <map>
#include <cmath>

#define formW 1080
#define formH 600
#define wireWidth 3.0
#define viaWidth 4.0
#define firstTrackPosition (24 + 3 + viaWidth/2) * lambda
#define trackGap 7 * lambda
#define netSteady 3

namespace AutoLayout {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace std;

	class component
	{
	public:
		component()
		{
			ratio = 1;
			polyNum = 0;
			gapNum = 0;
			height = baseCellLong * lambda;
			width = 0;
		};
		int ratio;
		string expr;
		int polyNum;
		int gapNum;
		int activeNum;
		float height;
		float width;
		vector<vector<material> > transistors;
		vector<material> pActive;
		vector<material> nActive;
		vector<material> poly;
		vector<material> metal1;
		vector<material> activeP;
		vector<material> activeN;
		vector<material> pselect;
		vector<material> nselect;
		vector<material> via;
		vector<material> m1Select;
		vector<material> m2Select;

		vector<column*> Top;
		vector<column*> Butt;

		vector<vector<path*> > net; // contain all the paths in the channel
		vector<track*> tracks;// contain all tracks
		map<int, track*> trackMap;

		
		vector<track *> aboutToFree;

	};

	/// <summary>
	/// MyForm1 摘要
	/// </summary>
	public ref class autoLayout : public System::Windows::Forms::Form
	{
	public:
		autoLayout(void)
		{
			InitializeComponent();
			//
			//TODO: 在此处添加构造函数代码
			//
			
		}
		autoLayout(MyForm ^temp)
		{
			InitializeComponent();
			//
			//TODO: 在此处添加构造函数代码
			//
			parent = temp;
			offsetX = 0;
			offsetY = 0;
			metalToMetal = 3;
			//formGraphics = pictureBox1->CreateGraphics();
		}
	private: System::Windows::Forms::Button^  button1;
	public: 
			System::Windows::Forms::PictureBox^  pictureBox1;

			MyForm ^parent;
			float offsetX;
			float offsetY;
			float metalToMetal;

			component *myComponent;
			void *temp;
			
			//Graphics^ formGraphics;

	protected:
		/// <summary>
		/// 清理所有正在使用的资源。
		/// </summary>
		~autoLayout()
		{
			if (components)
			{
				delete components;
			}
		}

	public: 
Void autoLayout::FillRectangleRectangle(Color clr, float a, float b, float c, float d);//System::Drawing::Rectangle^ rect)
Void autoLayout::DrawRectangleRectangle(float a, float b, float c, float d);//System::Drawing::Rectangle^ rect)
Void autoLayout::DrawLineLine(Color clr, float a, float b, float c, float d);

Void autoLayout::paraSet(void);
Void autoLayout::powerSet(void);
Void autoLayout::activeSet(void);
Void autoLayout::nActiveSet(void);
Void autoLayout::polySet(void);
Void autoLayout::pselectSet(void);
Void autoLayout::nselectSet(void);
Void autoLayout::FillMaterial(material);
Void autoLayout::FillPath(path *);
Void autoLayout::ShowNets(void);
Void autoLayout::drawLayout(void);
Void autoLayout::drawAllPath(void);
Void autoLayout::DrawStringString(string, float, float);
Void autoLayout::trackInitialize(void);


bool autoLayout::checkIfNeedPVia(string);
bool autoLayout::checkIfNeedNVia(string);
bool autoLayout::checkIfPower(string);
bool autoLayout::checkIfNeedNewTrack(column *);
bool autoLayout::checkIfNeededAnymore(string, unsigned int);
bool autoLayout::checkIfAlreadyInTrack(string);
bool autoLayout::checkIfNetExist(path *);
bool autoLayout::checkIfMerged(column *, track *);
bool autoLayout::checkTheTopNearest(column *, unsigned int);
bool autoLayout::checkTheButtNearest(column *, unsigned int);
bool autoLayout::checkIfIncluded(track *, vector<vector<track *> > &);
bool autoLayout::checkIfRaising(column *, unsigned int, track *);
bool autoLayout::checkIfDeleteVia(column *, string, float);

Void autoLayout::clean(void);
Void autoLayout::refreshPara(track *);
Void autoLayout::channelRouting(void);
Void autoLayout::assignTrackToTrack(column *, unsigned int, track *, track *);
Void autoLayout::assignTopToTrack(column *, track *, unsigned int);
Void autoLayout::assignButtToTrack(column *, track *, unsigned int);
Void autoLayout::assignTopToNewTrack(column *, unsigned int);
Void autoLayout::assignButtToNewTrack(column *, unsigned int);
Void autoLayout::createViaM1ToM2(path *, int);
Void autoLayout::generateVia(float, float);
Void autoLayout::step1(unsigned int);
Void autoLayout::step2(unsigned int);
Void autoLayout::step3(unsigned int);
Void autoLayout::step4(unsigned int);
Void autoLayout::step5(unsigned int);
Void autoLayout::step6(unsigned int);

track *autoLayout::findTheButtTrack(column *);
track *autoLayout::topToChoose(column *);
track *autoLayout::buttToChoose(column *);
track *autoLayout::createNewTrack(column *);

	private: System::ComponentModel::IContainer^  components;

	private:
		/// <summary>
		/// 必需的设计器变量。
		/// </summary>


#pragma region Windows Form Designer generated code
		/// <summary>
		/// 设计器支持所需的方法 - 不要
		/// 使用代码编辑器修改此方法的内容。
		/// </summary>
		void InitializeComponent(void)
		{
			this->button1 = (gcnew System::Windows::Forms::Button());
			this->pictureBox1 = (gcnew System::Windows::Forms::PictureBox());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox1))->BeginInit();
			this->SuspendLayout();
			// 
			// button1
			// 
			this->button1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->button1->Location = System::Drawing::Point(formW - 77, formH - 20);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(75, 18);
			this->button1->TabIndex = 0;
			this->button1->Text = L"Click Me";
			this->button1->UseVisualStyleBackColor = true;
			this->button1->Click += gcnew System::EventHandler(this, &autoLayout::button1_Click);
			// 
			// pictureBox1
			// 
			this->pictureBox1->BackColor = System::Drawing::Color::Black;
			this->pictureBox1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->pictureBox1->Location = System::Drawing::Point(0, 0);
			this->pictureBox1->Name = L"pictureBox1";
			this->pictureBox1->Size = System::Drawing::Size(formW, formH);
			this->pictureBox1->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			this->pictureBox1->TabIndex = 1;
			this->pictureBox1->TabStop = false;
			// 
			// autoLayout
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(formW, formH);
			this->Controls->Add(this->button1);
			this->Controls->Add(this->pictureBox1);
			this->MaximumSize = System::Drawing::Size(1280, 800);
			this->MinimumSize = System::Drawing::Size(formW, formH);
			this->Name = L"autoLayout";
			this->Text = L"autoLayout";
			this->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &autoLayout::Form1_DragDrop);
			this->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &autoLayout::Form1_DragDrop);
			this->FormClosed += gcnew System::Windows::Forms::FormClosedEventHandler(this, &autoLayout::clear);
			//this->DragEnter += new DragEventHandler(this.Form1_DragEnter);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox1))->EndInit();
			this->ResumeLayout(false);

		}
#pragma endregion
	private: System::Void button1_Click(System::Object^  sender, System::EventArgs^  e);
	//protected: virtual void OnResize(EventArgs^ e) override
	//		   {
	//			   drawLayout();
	//		   }

private: System::Void  Form1_DragDrop(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e)
{
	drawLayout();
   }

private: System::Void  clear(System::Object^ sender, System::Windows::Forms::FormClosedEventArgs^ e);


	};
}
