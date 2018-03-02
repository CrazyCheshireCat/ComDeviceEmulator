#include "ComDeviceEmulator.h"


bool ComDeviceEmulator::LoadData_UIM()
{
	return false;
	//Vector<int>            d_uim_nums;
	//Vector<Vector<double>> d_uim_vals;
	d_uim_nums.Add(1);
	d_uim_nums.Add(2);
	
	d_uim_vals.SetCount(d_uim_nums.GetCount() * 7);
	
	CSVParser csv;
	if (!csv.ParseFile(GetExeDirFile("uim_data.csv"))) {
		d_uim_nums.Clear();
		d_uim_vals.Clear();
		return false;
	}
	// Первая строчка - заголовок
	if (csv.GetCount_Row() < 1 || csv.GetCount_Col() < d_uim_vals.GetCount() + 1) {	
		d_uim_nums.Clear();
		d_uim_vals.Clear();
		return false;
	}
	// Первый столбец - время, следующие 7 * 2 столбцов - данные от УИМов
	for (int i = 1; i < csv.GetCount_Row(); ++i) {
		for (int j = 0; j < d_uim_vals.GetCount(); ++j) {
			d_uim_vals[j].Add(csv.GetCell_Double(i, j));
		}
	}
	if (d_uim_vals[0].GetCount() < 0) {
		d_uim_nums.Clear();
		d_uim_vals.Clear();		
		return false;
	}
	
	//d_uim_iter.SetCount(d_uim_vals.GetCount(), 0);
	for (int i = 0; i < d_uim_vals.GetCount(); ++i)
		d_uim_iter.Add(0);
	return true;
}

bool ComDeviceEmulator::IsValidUimAddress(int uim_num)
{
	if (d_uim_nums.GetCount() <= 0)
		return true;
	for (int i = 0; i < d_uim_nums.GetCount(); ++i) {
		if (d_uim_nums[i] == uim_num)
			return true;
	}
	return false;
}

static String GetRandomNumber()
{
	String res;
	res = FormatIntDec((Random() % 100000), 7, '0', true);
	res.Insert( Random() % 3 + 2, "." );
	return res;	
}
	
String ComDeviceEmulator::CatchRequest_UIM()
{
	// Если что-то есть - читаем это
	return d_com.Read();	
}

	
bool ComDeviceEmulator::SendResponse_UIM(const String& request)
{
	if (request.GetCount() < 4) return false;	// В запросе должно быть минимум 4 знака
	
	String response = GetResponseByRequest(request);
	if (response.IsEmpty()) return false;
	if (!d_com.Write(response)) {
		AddError("SendResponse COM" + FormatInt(d_cfg.com.GetValue()), "Can't write response to com");
		return false;
	}
	AddResponse(response);
	return true;	
}

double ComDeviceEmulator::GetUimValue(int uim_num, int ch_num)
{
	double v;
	DWORD r = Random() % 6801;
	v = ((double)r - 80.0) / 10.0;
	int it;
	for (int i = 0; i < d_uim_nums.GetCount(); ++i) {
		if (d_uim_nums[i] == uim_num) {
			it = (i*7)+ch_num;
			if (d_uim_iter[it] < d_uim_vals[it].GetCount()) {
				v = d_uim_vals[it][d_uim_iter[it]];
				d_uim_iter[it] = d_uim_iter[it] + 1;
			}
			break;
		}
	}
	return v;
}

static String FormatUimValue(double val)
{
	String v;
	//v  = (val >= 0 ? "+" : "-");
	v = FormatDoubleFix(val, 1, FD_SIGN);
	while (v.GetCount() < 7) 
		v.Insert(1,"0");
	return v;
}

String ComDeviceEmulator::GetResponseByRequest(const String& request)
{
	String num_str;
	// Второй и третий символы - номер устройства в hex
	num_str = request.Left(3);
	num_str = num_str.Right(2);
	// Теперь в num_str - номер устройства
	int uim_num = ScanInt(num_str, NULL, 16);
	// Проверяем адрес:
	if (!IsValidUimAddress(uim_num)) {
		AddError("GetResponseByRequest COM" + FormatInt(d_cfg.com.GetValue()), "Invalid UIM number: " + FormatInt(uim_num));
		return "";
	}
	String response;	
	// Ищем допустимые запросы:
	// Варианты с первым символом # в запросе - чтение измеряемых данных
	if (request[0] == '#') {
		response = "?" + num_str; // Вернет это, если запрос был неверный
		
		if (request.GetCount() == 5) {
			if (request[3] == '#') {
				if (request[4] >= '0' || request[4] <= '6') {
					response  = ">";
					response += num_str;
					response += request[4];
					response += FormatUimValue(GetUimValue(ScanInt(num_str), request[4] - '0'));//   GetRandomNumber();
				}
			}
		}
		if (request.GetCount() == 4) {
			// Если 4-й символ от 0 до 7 - идет запрос на чтение данных с одного канала
			if (request[3] >= '0' || request[3] <= '6') {
				response  = ">";
				//response += GetRandomNumber();
				response += FormatUimValue(GetUimValue(ScanInt(num_str), request[3] - '0'));
			}
			if (request[3] == '*') {
				response  = ">";
				response += num_str;
				response += request[3];
				for (int i = 0; i < 7; ++i) 
					response += FormatUimValue(GetUimValue(ScanInt(num_str), request[3] - '0')); //response += GetRandomNumber();
			}
		} 
	}
	//response += 0x0D;
	return response ;
}