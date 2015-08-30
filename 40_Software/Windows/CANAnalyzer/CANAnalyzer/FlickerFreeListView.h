#pragma once

#define WM_ERASEBKGND	0x0014

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

	public ref class FlickerFreeListView : public ListView
	{
	public:
		FlickerFreeListView()
		{
			//Activate double buffering
			SetStyle(ControlStyles::OptimizedDoubleBuffer | ControlStyles::AllPaintingInWmPaint, true);

			//Enable the OnNotifyMessage event so we get a chance to filter out 
			// Windows messages before they get to the form's WndProc
			SetStyle(ControlStyles::EnableNotifyMessage, true);
		}

	protected:
		virtual  void OnNotifyMessage(System::Windows::Forms::Message %m) override
		{
			//Filter out the WM_ERASEBKGND message
			if (m.Msg != WM_ERASEBKGND)
			{
				ListView::OnNotifyMessage(m);
			}
		}

	};
}
