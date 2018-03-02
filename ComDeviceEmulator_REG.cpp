#include "ComDeviceEmulator.h"

String ComDeviceEmulator::CatchRequest_Reg()
{
	return d_com.ask_com.Read(10);
}
	
bool ComDeviceEmulator::SendResponse_Reg(const String& request)
{
	String response;
	
	int x_o_r = request[0];
	for (int i = 1; i < request.GetCount(); ++i) 
		x_o_r = x_o_r ^ request[i];
	
	response += x_o_r;
	
	if (!d_com.Write(response)) {
		Log_AddError("SendResponse COM" + FormatInt(d_cfg.com.GetValue()), "Can't write response to com");
		return false;
	}
	//Log_AddResponse(response, true);
	Log_AddResponseHEX(response, true);
	return true;	
}