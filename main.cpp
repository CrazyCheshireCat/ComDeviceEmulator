#include "ComDeviceEmulator.h"

#define IMAGECLASS ComDeviceEmulatorImg
#define IMAGEFILE <ComDeviceEmulator/ComDeviceEmulator.iml>
#include <Draw/iml_source.h>

GUI_APP_MAIN
{
//	PromptOK(FormatDoubleFix(0.15, 1, FD_SIGN));
//	return;
	ComDeviceEmulator exe(ComDeviceEmulator::d_Regulator);
	//ComDeviceEmulator exe(ComDeviceEmulator::d_UIM);
	
	exe.Run();
}
