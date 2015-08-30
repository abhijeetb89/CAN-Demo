#pragma once
#include "libusb.h"
#include "CAN_DBC.h"
#include <iostream>
#include <string>
#include <msclr\marshal_cppstd.h>
#include <ctime>
#include <chrono>
#include <iomanip> 

using namespace std;
using namespace std::chrono;
using namespace System;

namespace CANAnalyzer{
	public class USB_Com
	{

	
	private:
		libusb_device_handle* usb_device_handle;
		vector<std::string> *can_Ids;
		vector<std::string> *can_MsgNames;
		vector<std::string> *can_Senders;
	
	private:
		string dectohex(int);
		bool searchDevice(libusb_device **dev, int pid, int *index, int devCount);
	
	public:
		USB_Com();
		bool USB_Com_Init();
		String^ USB_Com_Receive();
		void USB_Com_Deinit();
		void USB_Retrieve_Database();
		static void LIBUSB_CALL transfer_cb_fn(libusb_transfer* transfer);
		~USB_Com();
	};
}
