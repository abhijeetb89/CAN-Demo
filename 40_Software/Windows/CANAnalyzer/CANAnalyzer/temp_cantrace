#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include "libusb.h"
#include "USB_Com.h"
#include "CAN_DBC.h"
#include "SettingsForm.h"
#include "FlickerFreeListView.h"

namespace CANAnalyzer {

	using namespace System;
	using namespace System::IO;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::Threading;
	using namespace std;

	/// <summary>
	/// Summary for CANTrace
	/// </summary>
	public ref class CANTrace : public System::Windows::Forms::Form
	{
	public:
		CANTrace(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
		}


	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~CANTrace()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::ListView^  trace;

			 bool isStopped = true;
			 bool isStarted = false;
			 bool isPaused = false;
			 bool isUpdating = false;
			 bool isRelativeTimePressed = false;
			 bool time_flag = false;
			 StreamWriter^ outFile;
			 String^ filename;
			 USB_Com *usb_device = new USB_Com();
			 CAN_DBC^ dbc;
			 SettingsForm^ settings;

	protected:

	public: delegate void ListViewCallback(System::Object ^obj);

	protected:
	private: System::Windows::Forms::ColumnHeader^  Message;
	private: System::Windows::Forms::ColumnHeader^  Id;
	private: System::Windows::Forms::ColumnHeader^  Dlc;
	private: System::Windows::Forms::ColumnHeader^  Data;
	private: System::Windows::Forms::ColumnHeader^  Time;



	private: System::ComponentModel::BackgroundWorker^  backgroundWorker1;
	private: System::Windows::Forms::ToolStrip^  toolStrip1;
	private: System::Windows::Forms::ToolStripButton^  Start;
	private: System::Windows::Forms::ToolStripButton^  Pause;
	private: System::Windows::Forms::ToolStripButton^  Stop;
	private: System::Windows::Forms::ToolStripButton^  Clear;


	private: System::Windows::Forms::ToolStrip^  toolStrip2;
	private: System::Windows::Forms::ToolStripButton^  Settings;
	private: System::Windows::Forms::ColumnHeader^  Node;
	private: System::Windows::Forms::ToolStripButton^  RelativeTime;
	private: System::Windows::Forms::ToolStripButton^  ContinueUpdate;



	private: System::ComponentModel::IContainer^  components;

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>


#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(CANTrace::typeid));
			//this->trace = (gcnew System::Windows::Forms::ListView());
			this->trace = (gcnew FlickerFreeListView());
			this->Message = (gcnew System::Windows::Forms::ColumnHeader());
			this->Id = (gcnew System::Windows::Forms::ColumnHeader());
			this->Dlc = (gcnew System::Windows::Forms::ColumnHeader());
			this->Data = (gcnew System::Windows::Forms::ColumnHeader());
			this->Time = (gcnew System::Windows::Forms::ColumnHeader());
			this->Node = (gcnew System::Windows::Forms::ColumnHeader());
			this->backgroundWorker1 = (gcnew System::ComponentModel::BackgroundWorker());
			this->toolStrip1 = (gcnew System::Windows::Forms::ToolStrip());
			this->Start = (gcnew System::Windows::Forms::ToolStripButton());
			this->Pause = (gcnew System::Windows::Forms::ToolStripButton());
			this->Stop = (gcnew System::Windows::Forms::ToolStripButton());
			this->Clear = (gcnew System::Windows::Forms::ToolStripButton());
			this->toolStrip2 = (gcnew System::Windows::Forms::ToolStrip());
			this->Settings = (gcnew System::Windows::Forms::ToolStripButton());
			this->RelativeTime = (gcnew System::Windows::Forms::ToolStripButton());
			this->ContinueUpdate = (gcnew System::Windows::Forms::ToolStripButton());
			this->toolStrip1->SuspendLayout();
			this->toolStrip2->SuspendLayout();
			this->SuspendLayout();
			// 
			// trace
			// 
			this->trace->Columns->AddRange(gcnew cli::array< System::Windows::Forms::ColumnHeader^  >(6) {
				this->Message, this->Id, this->Dlc,
					this->Data, this->Time, this->Node
			});
			this->trace->Location = System::Drawing::Point(0, 85);
			this->trace->Name = L"trace";
			this->trace->Size = System::Drawing::Size(1074, 470);
			this->trace->TabIndex = 0;
			this->trace->UseCompatibleStateImageBehavior = false;
			this->trace->View = System::Windows::Forms::View::Details;
			// 
			// Message
			// 
			this->Message->Text = L"Message";
			this->Message->Width = 200;
			// 
			// Id
			// 
			this->Id->Text = L"ID";
			this->Id->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			this->Id->Width = 124;
			// 
			// Dlc
			// 
			this->Dlc->Text = L"DLC";
			this->Dlc->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			this->Dlc->Width = 102;
			// 
			// Data
			// 
			this->Data->Text = L"Data";
			this->Data->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			this->Data->Width = 332;
			// 
			// Time
			// 
			this->Time->Text = L"Time";
			this->Time->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			this->Time->Width = 172;
			// 
			// Node
			// 
			this->Node->Text = L"Node";
			this->Node->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			this->Node->Width = 142;
			// 
			// backgroundWorker1
			// 
			this->backgroundWorker1->WorkerSupportsCancellation = true;
			this->backgroundWorker1->DoWork += gcnew System::ComponentModel::DoWorkEventHandler(this, &CANTrace::backgroundWorker1_DoWork);
			this->backgroundWorker1->RunWorkerCompleted += gcnew System::ComponentModel::RunWorkerCompletedEventHandler(this, &CANTrace::backgroundWorker1_RunWorkerCompleted);
			// 
			// toolStrip1
			// 
			this->toolStrip1->AutoSize = false;
			this->toolStrip1->Dock = System::Windows::Forms::DockStyle::None;
			this->toolStrip1->ImageScalingSize = System::Drawing::Size(20, 20);
			this->toolStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(6) {
				this->Start, this->Pause,
					this->Stop, this->Clear, this->RelativeTime, this->ContinueUpdate
			});
			this->toolStrip1->Location = System::Drawing::Point(0, 43);
			this->toolStrip1->Name = L"toolStrip1";
			this->toolStrip1->Size = System::Drawing::Size(1074, 39);
			this->toolStrip1->Stretch = true;
			this->toolStrip1->TabIndex = 5;
			this->toolStrip1->TabStop = true;
			this->toolStrip1->Text = L"toolStrip1";
			// 
			// Start
			// 
			this->Start->AutoSize = false;
			this->Start->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Image;
			this->Start->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"Start.Image")));
			this->Start->ImageTransparentColor = System::Drawing::Color::Magenta;
			this->Start->Name = L"Start";
			this->Start->Size = System::Drawing::Size(33, 38);
			this->Start->Text = L"Start";
			this->Start->Click += gcnew System::EventHandler(this, &CANTrace::Start_Click_1);
			// 
			// Pause
			// 
			this->Pause->AutoSize = false;
			this->Pause->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Image;
			this->Pause->Enabled = false;
			this->Pause->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"Pause.Image")));
			this->Pause->ImageTransparentColor = System::Drawing::Color::Magenta;
			this->Pause->Name = L"Pause";
			this->Pause->Size = System::Drawing::Size(33, 38);
			this->Pause->Text = L"Pause";
			this->Pause->Click += gcnew System::EventHandler(this, &CANTrace::Pause_Click);
			// 
			// Stop
			// 
			this->Stop->AutoSize = false;
			this->Stop->AutoToolTip = false;
			this->Stop->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Image;
			this->Stop->Enabled = false;
			this->Stop->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"Stop.Image")));
			this->Stop->ImageTransparentColor = System::Drawing::Color::Magenta;
			this->Stop->Name = L"Stop";
			this->Stop->Size = System::Drawing::Size(33, 38);
			this->Stop->Text = L"Stop";
			this->Stop->Click += gcnew System::EventHandler(this, &CANTrace::Stop_Click_1);
			// 
			// Clear
			// 
			this->Clear->AutoSize = false;
			this->Clear->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Image;
			this->Clear->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"Clear.Image")));
			this->Clear->ImageTransparentColor = System::Drawing::Color::Magenta;
			this->Clear->Name = L"Clear";
			this->Clear->Size = System::Drawing::Size(33, 38);
			this->Clear->Text = L"Clear";
			this->Clear->Click += gcnew System::EventHandler(this, &CANTrace::Clear_Click_1);
			// 
			// toolStrip2
			// 
			this->toolStrip2->Dock = System::Windows::Forms::DockStyle::None;
			this->toolStrip2->ImageScalingSize = System::Drawing::Size(20, 20);
			this->toolStrip2->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) { this->Settings });
			this->toolStrip2->Location = System::Drawing::Point(0, 9);
			this->toolStrip2->Name = L"toolStrip2";
			this->toolStrip2->Size = System::Drawing::Size(78, 27);
			this->toolStrip2->TabIndex = 6;
			this->toolStrip2->Text = L"toolStrip2";
			// 
			// Settings
			// 
			this->Settings->BackColor = System::Drawing::SystemColors::ButtonFace;
			this->Settings->BackgroundImageLayout = System::Windows::Forms::ImageLayout::Center;
			this->Settings->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Text;
			this->Settings->ImageTransparentColor = System::Drawing::Color::Magenta;
			this->Settings->Name = L"Settings";
			this->Settings->Size = System::Drawing::Size(66, 24);
			this->Settings->Text = L"Settings";
			this->Settings->Click += gcnew System::EventHandler(this, &CANTrace::Settings_Click);
			// 
			// RelativeTime
			// 
			this->RelativeTime->AutoSize = false;
			this->RelativeTime->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Image;
			this->RelativeTime->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"RelativeTime.Image")));
			this->RelativeTime->ImageTransparentColor = System::Drawing::Color::Magenta;
			this->RelativeTime->Margin = System::Windows::Forms::Padding(0, 1, 2, 2);
			this->RelativeTime->Name = L"RelativeTime";
			this->RelativeTime->Size = System::Drawing::Size(24, 36);
			this->RelativeTime->Text = L"Relative Time";
			this->RelativeTime->Click += gcnew System::EventHandler(this, &CANTrace::RelativeTime_Click);
			// 
			// ContinueUpdate
			// 
			this->ContinueUpdate->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Image;
			this->ContinueUpdate->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"ContinueUpdate.Image")));
			this->ContinueUpdate->ImageTransparentColor = System::Drawing::Color::Magenta;
			this->ContinueUpdate->Name = L"ContinueUpdate";
			this->ContinueUpdate->Size = System::Drawing::Size(24, 36);
			this->ContinueUpdate->Text = L"ContinueUpdate";
			// 
			// CANTrace
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->AutoScroll = true;
			this->AutoSize = true;
			this->BackColor = System::Drawing::SystemColors::Menu;
			this->ClientSize = System::Drawing::Size(1074, 555);
			this->Controls->Add(this->toolStrip2);
			this->Controls->Add(this->toolStrip1);
			this->Controls->Add(this->trace);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedSingle;
			this->MaximizeBox = false;
			this->Name = L"CANTrace";
			this->Text = L"CANTrace";
			this->toolStrip1->ResumeLayout(false);
			this->toolStrip1->PerformLayout();
			this->toolStrip2->ResumeLayout(false);
			this->toolStrip2->PerformLayout();
			this->ResumeLayout(false);
			this->PerformLayout();

			settings = gcnew SettingsForm();

		}
#pragma endregion


	private: System::Void Start_Click_1(System::Object^  sender, System::EventArgs^  e) {

		if (isStopped){
			trace->Items->Clear();
		}

		Start->Enabled = false;
		Pause->Enabled = true;
		Stop->Enabled = true;

		if (!isPaused){

			time_flag = false;
			
			if (settings->getOverWriteCheckStatus())
			{
				filename = Path::Combine(settings->getTracePath(), (settings->getTraceName() + ".txt"));
			}
			else
			{
				std::fstream myfile;
				myfile.open("settings.txt", std::ios_base::in);
				int a;
				myfile >> a;
				myfile.close();

				myfile.open("settings.txt", std::ios_base::out | std::fstream::trunc);
				myfile << (a + 1);
				myfile.close();
				filename = Path::Combine(settings->getTracePath(), (settings->getTraceName() + "_" + a.ToString() + ".txt"));
			}

			outFile = gcnew StreamWriter(filename);
		}
		else if (isPaused){
			outFile = gcnew StreamWriter(filename, true);
		}

		isStarted = true;
		isPaused = false;
		isStopped = false;

		backgroundWorker1->RunWorkerAsync(1);


	}
	private: System::Void Pause_Click(System::Object^  sender, System::EventArgs^  e) {
		isPaused = true;
		Start->Enabled = true;
		Pause->Enabled = false;
		Stop->Enabled = true;
		backgroundWorker1->CancelAsync();	//stop background task
	}
	private: System::Void Stop_Click_1(System::Object^  sender, System::EventArgs^  e) {
		backgroundWorker1->CancelAsync();	//stop background task
		Start->Enabled = true;
		Pause->Enabled = false;
		Stop->Enabled = false;
		isStopped = true;
	}
	private: System::Void Clear_Click_1(System::Object^  sender, System::EventArgs^  e) {
		trace->Items->Clear();
	}


	private: void UpdateStatus(String^ msg) {

		String^ delimStr = ";!|$";
		array<Char>^ delimiter = delimStr->ToCharArray();
		array<String^>^ words;
		String^ traceline = "";
		int wordCount = 0;
		words = msg->Split(delimiter);

		static milliseconds current_time_ms;
		static milliseconds last_time_ms;
		static double absolute_time_ms = 0;
		static double relative_time_ms = 0;

		////////////////////////////////////////////////////////////////////
		// Timing Calculations
		////////////////////////////////////////////////////////////////////
		current_time_ms = duration_cast<milliseconds>(
			system_clock::now().time_since_epoch()
			);

		if (!time_flag)
		{
			last_time_ms = current_time_ms;
			time_flag = true;
			absolute_time_ms = 0;
			relative_time_ms = 0;
		}
		else
		{
			std::setprecision(4);
			absolute_time_ms += (((double)(current_time_ms.count() - last_time_ms.count())) / 1000);
			relative_time_ms = (((double)(current_time_ms.count() - last_time_ms.count())) / 1000);
			last_time_ms = current_time_ms;
		}
		//////////////////////////////////////////////////////////////////////
		
		
		trace->BeginUpdate();
		try{

			ListViewItem^ list = gcnew ListViewItem(words[wordCount++]);
			list->SubItems->Add(words[wordCount++]);
			list->SubItems->Add(words[wordCount++]);
			list->SubItems->Add(words[wordCount++]);
			
			if (!isRelativeTimePressed)
			{
				list->SubItems->Add(absolute_time_ms.ToString("N" + (settings->getTimePrecision()).ToString()));
			}
			else
			{
				list->SubItems->Add(relative_time_ms.ToString("N" + (settings->getTimePrecision()).ToString()));
			}
			
			list->SubItems->Add(words[wordCount++]);

			trace->Items->Add(list);
			
		}
		finally{

			trace->EndUpdate();
		}

		trace->Items[trace->Items->Count - 1]->EnsureVisible();

		for (int i = 0; i < wordCount; i++)
		{
			traceline += words[i] + "	";
		}

		outFile->WriteLine(traceline);

		this->Update();
	
	}



	private: System::Void backgroundWorker1_DoWork(System::Object^  sender, System::ComponentModel::DoWorkEventArgs^  e) {

		String^ stringData;

		if (!usb_device->USB_Com_Init())
		{
			backgroundWorker1->CancelAsync();
			e->Cancel = true;
			return;
		}


						
		do
		{
			//check if user has stopped button
			if (backgroundWorker1->CancellationPending)
			{
				e->Cancel = true;
				break;
			}
			else
			{
				stringData = usb_device->USB_Com_Receive();

				if (stringData != "")
				{
					Invoke(gcnew Action<String^>(this, &CANTrace::UpdateStatus), stringData);
				}
			}

		} while (1);

	}

	private: System::Void backgroundWorker1_RunWorkerCompleted
		(Object^ sender, System::ComponentModel::RunWorkerCompletedEventArgs^ e)
	{
		usb_device->USB_Com_Deinit();
		outFile->Close();
	}

	
	private: System::Void Settings_Click(System::Object^  sender, System::EventArgs^  e) {
		settings->ShowDialog();
	}

	private: System::Void RelativeTime_Click(System::Object^  sender, System::EventArgs^  e) {
		if (!isRelativeTimePressed)
		{
			isRelativeTimePressed = true;
			RelativeTime->Checked = true;
		}
		else
		{
			isRelativeTimePressed = false;
			RelativeTime->Checked = false;
		}
	}

};


}
