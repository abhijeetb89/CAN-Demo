#include "CAN_DBC.h"
using namespace CANAnalyzer;

CAN_DBC::CAN_DBC()
{
	form = gcnew SettingsForm();
	fileName = form->getDbcName();
	list_Msg_Names = gcnew List<String^>();
	list_Ids = gcnew List<String^>();
	list_Dlcs = gcnew List<String^>();
	list_CycleTimes = gcnew List<String^>();
	list_Senders = gcnew List<String^>();
}

bool CAN_DBC::CAN_DBC_Parse()
{

	try
	{
		XmlTextReader^ reader = gcnew XmlTextReader(fileName);
		int i = 0;
		msg_Count = 0;

		while (reader->Read())
		{
			switch (reader->NodeType)
			{
			case XmlNodeType::Element: break;
			case XmlNodeType::Text:
				
				switch (i)
				{
				case 0: list_Msg_Names->Add(reader->Value); i++; break;
				case 1: list_Ids->Add(reader->Value); i++; break;
				case 2: list_Dlcs->Add(reader->Value); i++;  break;
				case 3: list_CycleTimes->Add(reader->Value); i++;  break;
				case 4: list_Senders->Add(reader->Value);  msg_Count++; i = 0; break;
				}

				break;
			}
		}

		reader->Close();
	}
	catch (System::Xml::XmlException^  e)
	{
		
	}

	

	return false;
}

int CAN_DBC::CAN_DBC_GetMsgCount()
{
	return msg_Count;
}

List<String^>^ CAN_DBC::CAN_DBC_GetMsgNames()
{
	return list_Msg_Names;
}

List<String^>^ CAN_DBC::CAN_DBC_GetIds()
{
	return list_Ids;
}

List<String^>^ CAN_DBC::CAN_DBC_GetDlcs()
{
	return list_Dlcs;
}

List<String^>^ CAN_DBC::CAN_DBC_GetCycleTimes()
{
	return list_CycleTimes;
}

List<String^>^ CAN_DBC::CAN_DBC_GetSenders()
{
	return list_Senders;
}

bool CAN_DBC::CAN_DBC_GetData()
{
	return CAN_DBC_Parse();
}

CAN_DBC::~CAN_DBC()
{
}
