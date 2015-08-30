#include "USB_Com.h"
#include "CANTrace.h"
using namespace CANAnalyzer;
static void MarshalString(String ^ s, string& os);

USB_Com::USB_Com()
{
	can_Ids = new vector<string>;
	can_MsgNames = new vector<string>;
	can_Senders = new vector<string> ;
}


USB_Com::~USB_Com()
{
}

bool USB_Com::USB_Com_Init()
{
	int cnt = 0;
	int index = 0;
	const int pid = 22352;	//pid of stm32f4 discovery HID
	unsigned char buffer[11];
	int rcv_length = 0;

	libusb_context *context = NULL;
	libusb_device **devs;

	if (libusb_init(NULL))	//initialize USB
	{
		//throw exception

	}
	else
	{
		cnt = libusb_get_device_list(NULL, &devs);
		if (cnt <= 0)  //get the list of devices
		{
			//throw exception

		}
		else
		{
			if (!searchDevice(devs, pid, &index, cnt)) //search for device
			{
				return false;
			}
			else
			{
				if (libusb_open(devs[index], &usb_device_handle))		//open USB deivce
				{
					//throw exception
					return false;
				}
				else
				{
					if (libusb_claim_interface(usb_device_handle, 0))		//claim USB interface
					{
						//throw exception
						libusb_close(usb_device_handle);
						libusb_exit(NULL);
						return false;
					}
					else
					{
						//USB_Com_Receive();
						USB_Retrieve_Database();
						return true;
						/*
						libusb_transfer *irq_transfer = libusb_alloc_transfer(0);

						if (!irq_transfer)
							return -ENOMEM;

						libusb_fill_interrupt_transfer(irq_transfer, usb_device_handle, 0x81, buffer, sizeof(buffer), transfer_cb_fn, &rcv_length, 0);

						if (libusb_submit_transfer(irq_transfer))
							return 0;
						*/
						
					}
				}
			}
		}
	}
	return 0;
}

String^ USB_Com::USB_Com_Receive()
{
	unsigned char buffer[11];
	int rcv_length = 0;
	unsigned int received_CAN_Id;
	String^ stringData = "";
	String^ sender = "";
	

	if (libusb_interrupt_transfer(usb_device_handle, 0x81, buffer, sizeof(buffer), &rcv_length, 1000))	//no data is received
	{
		return "";
	}
	else		//data is received
	{

		//For Id
		received_CAN_Id = ((buffer[1] << 8) | buffer[0]);	//extract ID from first 2 bytes in buffer
		String^ id = gcnew String((dectohex(received_CAN_Id)).c_str());
		
		//For message
		if (can_Ids->end() != find(can_Ids->begin(), can_Ids->end(), (dectohex(received_CAN_Id)).c_str())
			)
		{
			int pos = find(can_Ids->begin(), can_Ids->end(), (dectohex(received_CAN_Id)).c_str()) - can_Ids->begin();
			string name = can_MsgNames->at(pos);
			String^ msgName = gcnew String(name.c_str());
			stringData += msgName + ";";
			sender = gcnew String(can_Senders->at(pos).c_str());
		}
		else
		{
			stringData += "" + ";";
		}

		stringData += id + "!";		//ID to string

		stringData += buffer[2].ToString() + "|";			//DLC to string

		for (int loop = 0; loop < (buffer[2]); loop++)
		{
			stringData += gcnew String(dectohex(buffer[loop + 3]).c_str()) + "  ";	//Data bytes to string
		}

		stringData += "$" + sender;

		return stringData;

	}

}

void USB_Com::USB_Retrieve_Database()
{
	CAN_DBC^ dbc_handler = gcnew CAN_DBC();
	dbc_handler->CAN_DBC_GetData();
	msclr::interop::marshal_context context;
	int msgCount = dbc_handler->CAN_DBC_GetMsgCount();
	std::string name;
	std::string id;
	std::string node;

	for (int i = 0; i < msgCount; i++)
	{
		MarshalString((dbc_handler->CAN_DBC_GetMsgNames())[i],  name);
		MarshalString((dbc_handler->CAN_DBC_GetIds())[i], id);
		MarshalString((dbc_handler->CAN_DBC_GetSenders())[i], node);

		can_MsgNames->push_back(name);
		can_Ids->push_back(id);
		can_Senders->push_back(node);
	}

}

string USB_Com::dectohex(int i)
{
	char hex_string[10];
	sprintf(hex_string, "%02X", i);
	std::string str = hex_string;
	return str;
}

bool USB_Com::searchDevice(libusb_device **dev, int pid, int *index, int devCount)
{
	libusb_device_descriptor desc;

	for (int count = 0; count < devCount; count++)
	{
		int r = libusb_get_device_descriptor(dev[count], &desc);	//get the device descriptor

		if (!r)
		{
			if (desc.idProduct == pid)		//pid of device is matched
			{
				*index = count;
				return true;
			}
		}
	}

	return false;
}

static void MarshalString(String ^ s, string& os) {
	using namespace Runtime::InteropServices;
	const char* chars =
		(const char*)(Marshal::StringToHGlobalAnsi(s)).ToPointer();
	os = chars;
	Marshal::FreeHGlobal(IntPtr((void*)chars));
}

void USB_Com::USB_Com_Deinit()
{
	libusb_close(usb_device_handle);
	libusb_exit(NULL);
}

void LIBUSB_CALL USB_Com::transfer_cb_fn(libusb_transfer* transfer)
{
	int cnt;
	unsigned char buffer[11];
	int rcv_length = 0;
	unsigned int received_CAN_Id;
	String^ stringData = "";
	String^ sender = "";

	if (transfer->status != LIBUSB_TRANSFER_COMPLETED)
	{
		cout << "Incomplete transfer" << endl;
	}
	else
	{
		//get data

	}
	if (libusb_submit_transfer(transfer) < 0){
		fprintf(stderr, "could not resubmit irq \n");
		exit(1);
	}



}