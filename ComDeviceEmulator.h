#ifndef _ComDeviceEmulator_ComDeviceEmulator_h
#define _ComDeviceEmulator_ComDeviceEmulator_h

#include <CtrlLib/CtrlLib.h>

using namespace Upp;

#define LAYOUTFILE <ComDeviceEmulator/ComDeviceEmulator.lay>
#include <CtrlCore/lay.h>

#define IMAGECLASS ComDeviceEmulatorImg
#define IMAGEFILE <ComDeviceEmulator/ComDeviceEmulator.iml>
#include <Draw/iml_header.h>

#include <SerialTalking/SerialTalking.h>
#include <HeapOfNeeds/Heap_CSV.h>

class ComDeviceEmulator : public WithComDeviceEmulatorLayout<TopWindow> 
{
	typedef ComDeviceEmulator CLASSNAME;
public:
	volatile Atomic terminated;
	volatile Atomic stop_listen;
	volatile Atomic threads;
	enum EmuDevs
	{
		d_UIM       = 0,
		d_Regulator = 1
	};
	
	ComDeviceEmulator(EmuDevs d);
	~ComDeviceEmulator();
	
	bool LoadConfig();

	bool IsComOpened() const { return d_com.IsOpened(); }
		
	void Listening();
	
	void Log_AddLine(String left_col, String right_col);
	void Log_AddError(String left_col, String right_col);
	void Log_AddRequest(String request, bool need_bytes = false);
	void Log_AddRequestHEX(String request, bool need_bytes = false);
	void Log_AddResponse(String response, bool need_bytes = false);
	void Log_AddResponseHEX(String response, bool need_bytes = false);	
	void Log_Update(AttrText left_col, AttrText right_col);
protected:
	EmuDevs      d_emu;
	AskSerial    d_com;
	ComConfigure d_cfg;  
	
	String GetButtonName() const;
	
	String GetResponseByRequest(const String& request);
	
	bool ComOpen()	         { ComClose(); return d_com.Open(d_cfg); }
	void ComClose()          { d_com.Close();                        }
	
	bool StartListening();
	bool StopListening();
	bool IsListening() const { return threads > 0;                       }
	bool SendResponse(const String& request);
	
	void PushBtn() { IsListening() ? StopListening() : StartListening(); }
	void CloseMe() { terminated = 1; Break();                            }
	
	void SetupByDevType();
private:
	Vector<int>            d_uim_nums;
	Vector<Vector<double>> d_uim_vals;
	Vector<int>            d_uim_iter;
	
	bool LoadData_UIM();
	
	String CatchRequest_UIM();
	String CatchRequest_Reg();
	
	bool SendResponse_UIM(const String& request);
	bool SendResponse_Reg(const String& request);
	
	bool IsValidUimAddress(int uim_num);
	double GetUimValue(int uim_num, int ch_num);
};

#endif
