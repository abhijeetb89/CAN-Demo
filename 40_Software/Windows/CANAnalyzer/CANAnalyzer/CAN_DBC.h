#pragma once

#include <fstream>
#include <vector>
#include "SettingsForm.h"

using namespace std;
using namespace System;
using namespace System::IO;
using namespace System::Collections::Generic;
using namespace System::Xml;
namespace CANAnalyzer{
	public ref class CAN_DBC
	{
	
	private: SettingsForm^ form;
	private: String^ fileName;
	private: List<String^>^ list_Msg_Names;
	private: List<String^>^ list_Ids;
	private: List<String^>^ list_Dlcs;
	private: List<String^>^ list_CycleTimes;
	private: List<String^>^ list_Senders;
	private: int msg_Count;
	private: bool CAN_DBC_Parse();

	public:
		CAN_DBC();
		bool CAN_DBC_GetData();
		int CAN_DBC_GetMsgCount();
		List<String^>^ CAN_DBC_GetMsgNames();
		List<String^>^ CAN_DBC_GetIds();
		List<String^>^ CAN_DBC_GetDlcs();
		List<String^>^ CAN_DBC_GetCycleTimes();
		List<String^>^ CAN_DBC_GetSenders();
		~CAN_DBC();
	};
}

