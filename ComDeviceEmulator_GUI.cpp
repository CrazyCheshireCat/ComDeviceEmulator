#include "ComDeviceEmulator.h"

void ComDeviceEmulator::Log_Update(AttrText left_col, AttrText right_col)
{
	if (wnd_log.GetCount() > 100) wnd_log.Clear();
	wnd_log.Add(left_col, right_col); 
	wnd_log.GoEnd();
}

void ComDeviceEmulator::Log_AddLine(String left_col, String right_col) 
{
	Log_Update(AttrText(left_col), AttrText(right_col));
}

void ComDeviceEmulator::Log_AddError(String left_col, String right_col)
{
	Log_Update(AttrText(left_col).Ink(Red()), AttrText(right_col).Ink(Red()));
}

static String ToBytesMes(const String& mes)
{
	String nums;
	for (int i = 0; i < mes.GetCount(); ++i) {
		nums += FormatInt(mes[i]);
		if (i < mes.GetCount() - 1) 
			nums += ":";
	}
	return nums;
}
void ComDeviceEmulator::Log_AddRequest(String request, bool need_bytes)
{
	if (need_bytes)
		request += (" [" + ToBytesMes(request) + "]");
	Log_Update(AttrText("ЗАПРОС: COM" + FormatInt(d_cfg.com.GetValue())).Bold().Ink(Green()),
			   AttrText(request).Ink(Gray()));
}

void ComDeviceEmulator::Log_AddRequestHEX(String request, bool need_bytes)
{
	if (need_bytes)
		request += (" [" + ToBytesMes(request) + "]");
	Log_Update(AttrText("ЗАПРОС: COM" + FormatInt(d_cfg.com.GetValue())).Bold().Ink(Green()),
			   AttrText(request).Ink(Gray()));

}
		
void ComDeviceEmulator::Log_AddResponse(String response, bool need_bytes)
{
	if (need_bytes)
		response += (" [" + ToBytesMes(response) + "]");
	Log_Update(AttrText("ОТВЕТ: COM" + FormatInt(d_cfg.com.GetValue())).Bold().Ink(Blue()),
			   AttrText(response).Ink(Gray()))	;
}

void ComDeviceEmulator::Log_AddResponseHEX(String response, bool need_bytes)
{
	String resp;
	if (need_bytes)
		resp += (" [" + ToBytesMes(response) + "]");
	Log_Update(AttrText("ОТВЕТ: COM" + FormatInt(d_cfg.com.GetValue())).Bold().Ink(Blue()),
			   AttrText(resp).Ink(Gray()))	;
}
