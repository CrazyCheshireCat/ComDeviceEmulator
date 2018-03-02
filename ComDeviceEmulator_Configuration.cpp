#include "ComDeviceEmulator.h"

bool ComDeviceEmulator::LoadConfig()
{
	String path = GetExeDirFile("comdeviceemulator.cfg");
	if (!FileExists( path )) {
		AddError("LoadConfig", "Can't find config file");
		return false;
	}
	try {
		String  s = LoadFile(path);
		CParser p(s);
		Value  j_conf = ParseJSON(p);
		
		if (!d_cfg.SetJson(j_conf)) {
			AddError("LoadConfig", "Bad config file");
			AddError("LoadConfig - com config", d_cfg.GetConfigError());
			return false;
		}
 	} catch (CParser::Error) {
		AddError("LoadConfig", "CParser::Error"); 
		return false;
	} catch (...) {
		AddError("LoadConfig", "Unknown parser error"); 
		return false;
	}
	return true;
}