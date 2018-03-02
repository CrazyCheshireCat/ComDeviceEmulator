#include "ComDeviceEmulator.h"

String ComDeviceEmulator::GetButtonName() const
{
	String str;
	if (d_com.IsOpened()) 
		str = "Остановить";
	else
		str = "Начать";
	str += " эмуляцию устройства на COM" + FormatInt(d_cfg.com.GetValue());
	return str;
}

ComDeviceEmulator::ComDeviceEmulator(EmuDevs d)
{
	d_emu = d;
	SetLanguage(LNGC_('r','u','R','U', CHARSET_UTF8));
	CtrlLayout(*this, "Эмулятор");
	
	Sizeable();
	MinimizeBox();
	
	this->WhenClose = THISBACK(CloseMe);
	
	// Потоки
	terminated  = 0;
	stop_listen = 0;
	threads     = 0;
	
	// Две колонки в таблице:
	wnd_log.AddColumn("", 1);
	wnd_log.AddColumn("", 3);
	
	// Грузим конфигурацию:
	if (!LoadConfig()) {
		btn_start.SetLabel("Ошибка конфигурации!");
		btn_start.Enable(false);
		SetTimeCallback(-10000, callback(this, &ComDeviceEmulator::CloseMe));
		return;
	}
	
	SetupByDevType();
	
	btn_start.SetLabel(GetButtonName());
	btn_start.Enable(true);
	
	btn_start <<= THISBACK(PushBtn);
	
	
	wnd_log.NoCursor();
	// 
//	dev_com.SetEndMessageSymbol(0x0D);
//	dev_com.SetHideEndMessageSymbol();
}

ComDeviceEmulator::~ComDeviceEmulator()
{
	while(threads)
		Sleep(1);
	ComClose();
}

void ComDeviceEmulator::SetupByDevType()
{
	if (d_emu == d_UIM) {
		Title("Эмулятор УИМ");
		d_com.SetEndMessageSymbol(0x0D);
		d_com.SetHideEndMessageSymbol();
		if (!LoadData_UIM()) {
			Log_AddLine("Данные для эмуляции", "Файл с данными не найден");
			Log_AddLine("Данные для эмуляции", "Поток данных - случайные числа");
		}
	}
	if (d_emu == d_Regulator) {
		Title("Эмулятор Регулятора"); 
		
	}
}

void Thread_StartComListening(ComDeviceEmulator* gui)
{
	if (gui->IsComOpened()) {
		gui->Listening();
	}
	AtomicDec(gui->threads);
}

bool ComDeviceEmulator::StartListening()
{
	// 
	ComClose();
	if (!ComOpen()) {
		Log_AddError("StartListening", "Can't open COM" + FormatInt(d_cfg.com.GetValue()) + ": " + d_com.GetError());
		return false;
	}	
	// Переименовываем кнопку:
	btn_start.SetLabel(GetButtonName());
	// Запускаем отдельный поток для прослушивания порта:
	AtomicInc(threads);
	Thread().Run(callback1(Thread_StartComListening,this));
	return true;
}


void ComDeviceEmulator::Listening()
{
	Log_AddLine("Listening", "START");
	String request;
	for (;;){
		Sleep(1);
		if (stop_listen)    { AtomicDec(stop_listen); break; }
		if (terminated)     { break;                         }
		if (!IsComOpened()) { break;                         }
		// Если нечего читать - спим и на следующий цикл
		if (!d_com.IsAvailable()) { continue;   }
		
		switch (d_emu) {
			case (d_UIM)       : request = CatchRequest_UIM(); Log_AddRequest(request, false); break;
			case (d_Regulator) : request = CatchRequest_Reg(); Log_AddRequest(request, true);  break;
		}

		if (!request.IsEmpty()) {
			if (!SendResponse(request))
				Log_AddError("COM" + FormatInt(d_cfg.com.GetValue()), "Ответ не может быть отправлен");
		}
		request.Clear();
	}
	Log_AddLine("Listening", "STOP");
}

bool ComDeviceEmulator::StopListening()
{
	if (threads == 0) return false;
	if (stop_listen)  return false;
	
	AtomicInc(stop_listen);
	while(stop_listen) Sleep(1);
	
	ComClose();
	
	btn_start.SetLabel(GetButtonName());
	return true;
}

bool ComDeviceEmulator::SendResponse(const String& request)
{
	if (request.IsEmpty()) return false;

	switch (d_emu) {
		case (d_UIM)       : return SendResponse_UIM(request);
		case (d_Regulator) : return SendResponse_Reg(request);
	}
	return false;	
}
