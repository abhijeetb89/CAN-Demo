#include <iostream>
#include "libusb.h"
#include "CANTrace.h"
#include "SettingsForm.h"

using namespace std;
using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;
using namespace CANAnalyzer;


[STAThread]
int main()
{

	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);

	CANTrace myTrace;
	Application::Run(%myTrace);


	return 0;
}




