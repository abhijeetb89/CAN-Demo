#pragma once

namespace CANAnalyzer {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::Threading;

	/// <summary>
	/// Summary for MyForm
	/// </summary>
	public ref class SettingsForm : public System::Windows::Forms::Form
	{
	public:
		SettingsForm(void)
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
		~SettingsForm()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::CheckBox^  checkBox_OverwriteTrace;
	protected:

	protected:
	private: System::Windows::Forms::Button^  button1;
	private: System::Windows::Forms::TextBox^  textBox_Dbc;


	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::TextBox^  textBox_TracePath;


	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::Button^  button2;
	private: System::Windows::Forms::OpenFileDialog^  OpenDbcDialog;

	private: static String^ dbc_name = gcnew String("");
	private: static String^ trace_name = gcnew String("");
	private: static String^ trace_path = gcnew String("");
	private: static bool overwrite_checked = false;
	private: static int time_precision = 1;
	private: System::Windows::Forms::OpenFileDialog^  OpenTraceDialog;
	private: System::Windows::Forms::FolderBrowserDialog^  TraceFolderBrowser;
	private: System::Windows::Forms::TextBox^  textBox_TraceName;

	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::Button^  Settings_Ok;
	private: System::Windows::Forms::Button^  Settings_Cancel;
	private: System::Windows::Forms::Label^  TimePrecision;
	private: System::Windows::Forms::NumericUpDown^  PrecisionUpDown;



	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->checkBox_OverwriteTrace = (gcnew System::Windows::Forms::CheckBox());
			this->button1 = (gcnew System::Windows::Forms::Button());
			this->textBox_Dbc = (gcnew System::Windows::Forms::TextBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->textBox_TracePath = (gcnew System::Windows::Forms::TextBox());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->button2 = (gcnew System::Windows::Forms::Button());
			this->OpenDbcDialog = (gcnew System::Windows::Forms::OpenFileDialog());
			this->OpenTraceDialog = (gcnew System::Windows::Forms::OpenFileDialog());
			this->TraceFolderBrowser = (gcnew System::Windows::Forms::FolderBrowserDialog());
			this->textBox_TraceName = (gcnew System::Windows::Forms::TextBox());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->Settings_Ok = (gcnew System::Windows::Forms::Button());
			this->Settings_Cancel = (gcnew System::Windows::Forms::Button());
			this->TimePrecision = (gcnew System::Windows::Forms::Label());
			this->PrecisionUpDown = (gcnew System::Windows::Forms::NumericUpDown());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->PrecisionUpDown))->BeginInit();
			this->SuspendLayout();
			// 
			// checkBox_OverwriteTrace
			// 
			this->checkBox_OverwriteTrace->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->checkBox_OverwriteTrace->AutoSize = true;
			this->checkBox_OverwriteTrace->Location = System::Drawing::Point(27, 180);
			this->checkBox_OverwriteTrace->Name = L"checkBox_OverwriteTrace";
			this->checkBox_OverwriteTrace->RightToLeft = System::Windows::Forms::RightToLeft::Yes;
			this->checkBox_OverwriteTrace->Size = System::Drawing::Size(199, 21);
			this->checkBox_OverwriteTrace->TabIndex = 0;
			this->checkBox_OverwriteTrace->Text = L"Overwrite existing trace file";
			this->checkBox_OverwriteTrace->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			this->checkBox_OverwriteTrace->UseVisualStyleBackColor = true;
			this->checkBox_OverwriteTrace->CheckedChanged += gcnew System::EventHandler(this, &SettingsForm::checkBox1_CheckedChanged);
			// 
			// button1
			// 
			this->button1->Location = System::Drawing::Point(667, 49);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(31, 22);
			this->button1->TabIndex = 1;
			this->button1->Text = L"...";
			this->button1->UseVisualStyleBackColor = true;
			this->button1->Click += gcnew System::EventHandler(this, &SettingsForm::button1_Click);
			// 
			// textBox_Dbc
			// 
			this->textBox_Dbc->Location = System::Drawing::Point(186, 50);
			this->textBox_Dbc->Name = L"textBox_Dbc";
			this->textBox_Dbc->Size = System::Drawing::Size(460, 22);
			this->textBox_Dbc->TabIndex = 2;
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(24, 52);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(156, 17);
			this->label1->TabIndex = 3;
			this->label1->Text = L"Add CAN Database File";
			// 
			// textBox_TracePath
			// 
			this->textBox_TracePath->Location = System::Drawing::Point(186, 94);
			this->textBox_TracePath->Name = L"textBox_TracePath";
			this->textBox_TracePath->Size = System::Drawing::Size(460, 22);
			this->textBox_TracePath->TabIndex = 2;
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(24, 139);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(112, 17);
			this->label2->TabIndex = 5;
			this->label2->Text = L"Trace File Name";
			// 
			// button2
			// 
			this->button2->Location = System::Drawing::Point(667, 97);
			this->button2->Name = L"button2";
			this->button2->Size = System::Drawing::Size(31, 22);
			this->button2->TabIndex = 6;
			this->button2->Text = L"...";
			this->button2->UseVisualStyleBackColor = true;
			this->button2->Click += gcnew System::EventHandler(this, &SettingsForm::button2_Click);
			// 
			// OpenDbcDialog
			// 
			this->OpenDbcDialog->FileName = L"OpenDbcDialog";
			// 
			// OpenTraceDialog
			// 
			this->OpenTraceDialog->FileName = L"OpenTraceDialog";
			// 
			// textBox_TraceName
			// 
			this->textBox_TraceName->Location = System::Drawing::Point(186, 136);
			this->textBox_TraceName->Name = L"textBox_TraceName";
			this->textBox_TraceName->Size = System::Drawing::Size(460, 22);
			this->textBox_TraceName->TabIndex = 7;
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(24, 100);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(104, 17);
			this->label3->TabIndex = 8;
			this->label3->Text = L"Trace File Path";
			// 
			// Settings_Ok
			// 
			this->Settings_Ok->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->Settings_Ok->Location = System::Drawing::Point(513, 274);
			this->Settings_Ok->Name = L"Settings_Ok";
			this->Settings_Ok->Size = System::Drawing::Size(95, 38);
			this->Settings_Ok->TabIndex = 9;
			this->Settings_Ok->Text = L"OK";
			this->Settings_Ok->UseVisualStyleBackColor = true;
			this->Settings_Ok->Click += gcnew System::EventHandler(this, &SettingsForm::Settings_Ok_Click);
			// 
			// Settings_Cancel
			// 
			this->Settings_Cancel->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->Settings_Cancel->Location = System::Drawing::Point(632, 275);
			this->Settings_Cancel->Name = L"Settings_Cancel";
			this->Settings_Cancel->Size = System::Drawing::Size(95, 38);
			this->Settings_Cancel->TabIndex = 9;
			this->Settings_Cancel->Text = L"Cancel";
			this->Settings_Cancel->UseVisualStyleBackColor = true;
			this->Settings_Cancel->Click += gcnew System::EventHandler(this, &SettingsForm::Settings_Cancel_Click);
			// 
			// TimePrecision
			// 
			this->TimePrecision->AutoSize = true;
			this->TimePrecision->Location = System::Drawing::Point(27, 222);
			this->TimePrecision->Name = L"TimePrecision";
			this->TimePrecision->Size = System::Drawing::Size(101, 17);
			this->TimePrecision->TabIndex = 10;
			this->TimePrecision->Text = L"Time Precision";
			// 
			// PrecisionUpDown
			// 
			this->PrecisionUpDown->Location = System::Drawing::Point(186, 222);
			this->PrecisionUpDown->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 7, 0, 0, 0 });
			this->PrecisionUpDown->Name = L"PrecisionUpDown";
			this->PrecisionUpDown->Size = System::Drawing::Size(52, 22);
			this->PrecisionUpDown->TabIndex = 11;
			this->PrecisionUpDown->ValueChanged += gcnew System::EventHandler(this, &SettingsForm::PrecisionUpDown_ValueChanged);
			// 
			// SettingsForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(772, 335);
			this->Controls->Add(this->PrecisionUpDown);
			this->Controls->Add(this->TimePrecision);
			this->Controls->Add(this->Settings_Cancel);
			this->Controls->Add(this->Settings_Ok);
			this->Controls->Add(this->label3);
			this->Controls->Add(this->textBox_TraceName);
			this->Controls->Add(this->button2);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->textBox_TracePath);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->textBox_Dbc);
			this->Controls->Add(this->button1);
			this->Controls->Add(this->checkBox_OverwriteTrace);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedSingle;
			this->Name = L"SettingsForm";
			this->Text = L"Settings";
			this->Load += gcnew System::EventHandler(this, &SettingsForm::SettingsForm_Load);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->PrecisionUpDown))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	
	private: System::Void button1_Click(System::Object^  sender, System::EventArgs^  e) {
		
		OpenDbcDialog->ShowDialog();
		textBox_Dbc->Text = OpenDbcDialog->FileName;
		/*
		SettingsForm^ form = gcnew SettingsForm();
		System::Threading::Thread^ t = gcnew
		System::Threading::Thread(gcnew ThreadStart(form, &SettingsForm::ThreadProcShowDialog));
		t->SetApartmentState(System::Threading::ApartmentState::STA);
		t->Start();
		t->Join();
		return;
		*/
		
	}
	
	private: System::Void button2_Click(System::Object^  sender, System::EventArgs^  e) {
		TraceFolderBrowser->ShowDialog();
		textBox_TracePath->Text = TraceFolderBrowser->SelectedPath;
	}

	private: System::Void checkBox1_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
		overwrite_checked = checkBox_OverwriteTrace->Checked;
	}

	private: Void ThreadProcShowDialog()
	{
		 OpenDbcDialog->ShowDialog();
		 textBox_Dbc->Text = OpenDbcDialog->FileName;
		 setDbcName(OpenDbcDialog->FileName);
	}

	public: Void setDbcName(String^ name)
	{
		dbc_name = name;
	}

	public: String^ getDbcName()
	{
		return dbc_name;
	}

	public: Void setTraceName(String^ name)
	{
		trace_name = name;
	}

	public: String^ getTraceName()
	{
		return textBox_TraceName->Text;
	}

	public: Void setTracePath(String^ path)
	{
		trace_path = path;
	}

	public: String^ getTracePath()
	{
		return trace_path;
	}

	public: Void setOverWriteCheckStatus(bool status)
	{
		checkBox_OverwriteTrace->Checked = status;
	}

	public: bool getOverWriteCheckStatus()
	{
		return overwrite_checked;
	}

	public: Void setTimePrecision(int precision)
	{
		time_precision = precision;
	}
	
	public: int getTimePrecision()
	{
		return time_precision;
	}
	
	private: System::Void SettingsForm_Load(System::Object^  sender, System::EventArgs^  e) {
		//retrieve the last settings
		textBox_Dbc->Text = dbc_name;
		textBox_TracePath->Text = trace_path;
		textBox_TraceName->Text = trace_name;
		checkBox_OverwriteTrace->Checked = overwrite_checked;
		PrecisionUpDown->Value = time_precision;
	}
	
	private: System::Void Settings_Ok_Click(System::Object^  sender, System::EventArgs^  e) {
		//save the settings
		dbc_name = textBox_Dbc->Text;
		trace_path = textBox_TracePath->Text;
		trace_name = textBox_TraceName->Text;
		overwrite_checked = checkBox_OverwriteTrace->Checked;
		time_precision = (int)PrecisionUpDown->Value;
		this->Close();
	}

	private: System::Void Settings_Cancel_Click(System::Object^  sender, System::EventArgs^  e) {
		//close the form without saving any changes
		this->Close();
	}
	
	private: System::Void PrecisionUpDown_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
		time_precision = (int)PrecisionUpDown->Value;
	}
};
}
