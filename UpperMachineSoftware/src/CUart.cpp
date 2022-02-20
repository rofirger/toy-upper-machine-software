// CUart.cpp: 实现文件
//

#include "pch.h"
#include "UpperMachineSoftware.h"
#include "CUart.h"
#include "afxdialogex.h"
#include <vector>
bool is_end_data_ac_func_uart = false;
IMPLEMENT_DYNAMIC(CUart, CDialogEx)

CUart::~CUart()
{
}

void CUart::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_COMS, m_combo_coms);
	DDX_Control(pDX, IDC_COMBO_BAUDRATE, m_combo_baudrate);
	DDX_Control(pDX, IDC_COMBO_DATABIT, m_combo_databit);
	DDX_Control(pDX, IDC_COMBO_CHECK, m_combo_check);
	DDX_Control(pDX, IDC_COMBO_STOPBIT, m_combo_stopbit);
}


BEGIN_MESSAGE_MAP(CUart, CDialogEx)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_MAIN_BUTTON, &CUart::OnBnClickedMainButton)
END_MESSAGE_MAP()


void CUart::ReadCom()
{
	HKEY h_key;
	wchar_t port_name[256];
	UCHAR com_name[256];
	// 打开串口注册表对应的键值  
	if (ERROR_SUCCESS == ::RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("Hardware\\DeviceMap\\SerialComm"), NULL, KEY_READ, &h_key))
	{
		int i = 0;
		int mm = 0;
		DWORD  dw_long, dw_size;
		while (TRUE)
		{
			dw_long = dw_size = sizeof(port_name);
			// 枚举串口
			if (ERROR_NO_MORE_ITEMS == ::RegEnumValue(h_key, i, port_name, &dw_long, NULL, NULL, (PUCHAR)com_name, &dw_size))
			{
				break;
			}
			CString temp;
			temp.Format(_T("%s"), com_name);
			m_combo_coms.AddString(temp);
			i++;
		}
		// 关闭注册表
		RegCloseKey(h_key);
		m_combo_coms.SetCurSel(0);
	}
	else
	{
		rofirger::add_log(rofirger::LOG_LEVEL_FATAL, "FATAL: REGISTRY error: {Hardware\\DeviceMap\\SerialComm} can't be accessed");
		::MessageBox(NULL, _T("注册表错误!"), _T("警告"), 0);
	}
}

void CUart::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	// 串口配置
	CRect client_rect;
	CRect rect_static_border;
	GetClientRect(&client_rect);
	CWnd* pWnd_static_border = GetDlgItem(IDC_STATIC_BORDER);

	CWnd* pWnd_coms = GetDlgItem(IDC_COMBO_COMS);
	CWnd* pWnd_baudrate = GetDlgItem(IDC_COMBO_BAUDRATE);
	CWnd* pWnd_databits = GetDlgItem(IDC_COMBO_DATABIT);
	CWnd* pWnd_check = GetDlgItem(IDC_COMBO_CHECK);
	CWnd* pWnd_stopbit = GetDlgItem(IDC_COMBO_STOPBIT);
	CWnd* pWnd_static_1 = GetDlgItem(IDC_STATIC_1);
	CWnd* pWnd_static_2 = GetDlgItem(IDC_STATIC_2);
	CWnd* pWnd_static_3 = GetDlgItem(IDC_STATIC_3);
	CWnd* pWnd_static_4 = GetDlgItem(IDC_STATIC_4);
	CWnd* pWnd_static_5 = GetDlgItem(IDC_STATIC_5);
	CWnd* pWnd_button = GetDlgItem(IDC_MAIN_BUTTON);
	if (pWnd_static_1 && pWnd_static_2 && pWnd_static_3 && pWnd_static_4 && pWnd_static_5)
	{
		pWnd_static_1->GetWindowRect(&rect_s_1);
		ScreenToClient(&rect_s_1);
		pWnd_static_2->GetWindowRect(&rect_s_2);
		ScreenToClient(&rect_s_2);
		pWnd_static_3->GetWindowRect(&rect_s_3);
		ScreenToClient(&rect_s_3);
		pWnd_static_4->GetWindowRect(&rect_s_4);
		ScreenToClient(&rect_s_4);
		pWnd_static_5->GetWindowRect(&rect_s_5);
		ScreenToClient(&rect_s_5);
		pWnd_coms->GetWindowRect(&rect_coms);
		ScreenToClient(&rect_coms);
		pWnd_baudrate->GetWindowRect(&rect_baudrate);
		ScreenToClient(&rect_baudrate);
		pWnd_databits->GetWindowRect(&rect_databits);
		ScreenToClient(&rect_databits);
		pWnd_check->GetWindowRect(&rect_check);
		ScreenToClient(&rect_check);
		pWnd_stopbit->GetWindowRect(&rect_stopbit);
		ScreenToClient(&rect_stopbit);
		pWnd_button->GetWindowRect(&rect_button);
		ScreenToClient(&rect_button);
	}

	if (pWnd_static_border)
	{
		pWnd_static_border->SetWindowPos(NULL, client_rect.left + 20,
			client_rect.top + 20,
			rect_s_1.Width() + 50 + rect_coms.Width(),
			rect_coms.Height() * 5 + rect_button.Height() + 130,
			SWP_NOZORDER);
		pWnd_static_border->GetWindowRect(&rect_static_border);
		ScreenToClient(&rect_static_border);

		pWnd_static_1->SetWindowPos(NULL, rect_static_border.left + 25, rect_static_border.top + 35, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		pWnd_static_1->GetWindowRect(&rect_s_1);
		ScreenToClient(&rect_s_1);
		pWnd_coms->SetWindowPos(NULL, rect_s_1.right, rect_s_1.top - (rect_coms.Height() - rect_s_1.Height()) / 2, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		pWnd_coms->GetWindowRect(&rect_coms);
		ScreenToClient(&rect_coms);

		pWnd_static_2->SetWindowPos(NULL, rect_static_border.left + 25, rect_s_1.bottom + 15, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		pWnd_static_2->GetWindowRect(&rect_s_2);
		ScreenToClient(&rect_s_2);
		pWnd_baudrate->SetWindowPos(NULL, rect_s_1.right, rect_s_2.top - (rect_baudrate.Height() - rect_s_2.Height()) / 2, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		pWnd_baudrate->GetWindowRect(&rect_baudrate);
		ScreenToClient(&rect_baudrate);

		pWnd_static_3->SetWindowPos(NULL, rect_static_border.left + 25, rect_s_2.bottom + 15, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		pWnd_static_3->GetWindowRect(&rect_s_3);
		ScreenToClient(&rect_s_3);
		pWnd_databits->SetWindowPos(NULL, rect_s_1.right, rect_s_3.top - (rect_databits.Height() - rect_s_3.Height()) / 2, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		pWnd_databits->GetWindowRect(&rect_databits);
		ScreenToClient(&rect_databits);

		pWnd_static_4->SetWindowPos(NULL, rect_static_border.left + 25, rect_s_3.bottom + 15, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		pWnd_static_4->GetWindowRect(&rect_s_4);
		ScreenToClient(&rect_s_4);
		pWnd_check->SetWindowPos(NULL, rect_s_1.right, rect_s_4.top - (rect_check.Height() - rect_s_4.Height()) / 2, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		pWnd_check->GetWindowRect(&rect_check);
		ScreenToClient(&rect_check);

		pWnd_static_5->SetWindowPos(NULL, rect_static_border.left + 25, rect_s_4.bottom + 15, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		pWnd_static_5->GetWindowRect(&rect_s_5);
		ScreenToClient(&rect_s_5);
		pWnd_stopbit->SetWindowPos(NULL, rect_s_1.right, rect_s_5.top - (rect_stopbit.Height() - rect_s_5.Height()) / 2, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		pWnd_stopbit->GetWindowRect(&rect_stopbit);
		ScreenToClient(&rect_stopbit);

		pWnd_button->SetWindowPos(NULL, rect_static_border.left + 40, rect_s_5.bottom + 50, rect_static_border.Width() - 80, rect_coms.Height() + 18, SWP_NOZORDER);
		pWnd_button->GetWindowRect(&rect_button);
		ScreenToClient(&rect_button);
	}
	// TODO: 在此处添加消息处理程序代码
}


BOOL CUart::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	ReadCom();
	m_combo_baudrate.AddString(CString(std::string(std::to_string(CBR_110)).c_str()));
	m_combo_baudrate.AddString(CString(std::string(std::to_string(CBR_300)).c_str()));
	m_combo_baudrate.AddString(CString(std::string(std::to_string(CBR_600)).c_str()));
	m_combo_baudrate.AddString(CString(std::string(std::to_string(CBR_1200)).c_str()));
	m_combo_baudrate.AddString(CString(std::string(std::to_string(CBR_2400)).c_str()));
	m_combo_baudrate.AddString(CString(std::string(std::to_string(CBR_4800)).c_str()));
	m_combo_baudrate.AddString(CString(std::string(std::to_string(CBR_9600)).c_str()));
	m_combo_baudrate.AddString(CString(std::string(std::to_string(CBR_14400)).c_str()));
	m_combo_baudrate.AddString(CString(std::string(std::to_string(CBR_19200)).c_str()));
	m_combo_baudrate.AddString(CString(std::string(std::to_string(CBR_38400)).c_str()));
	m_combo_baudrate.AddString(CString(std::string(std::to_string(CBR_56000)).c_str()));
	m_combo_baudrate.AddString(CString(std::string(std::to_string(CBR_57600)).c_str()));
	m_combo_baudrate.AddString(CString(std::string(std::to_string(CBR_115200)).c_str()));
	m_combo_baudrate.AddString(CString(std::string(std::to_string(CBR_128000)).c_str()));
	m_combo_baudrate.AddString(CString(std::string(std::to_string(CBR_256000)).c_str()));
	m_combo_baudrate.SetCurSel(6);

	m_combo_databit.AddString(CString(std::string(std::to_string(5)).c_str()));
	m_combo_databit.AddString(CString(std::string(std::to_string(6)).c_str()));
	m_combo_databit.AddString(CString(std::string(std::to_string(7)).c_str()));
	m_combo_databit.AddString(CString(std::string(std::to_string(8)).c_str()));
	m_combo_databit.SetCurSel(3);

	m_combo_check.AddString(CString("无"));
	m_combo_check.AddString(CString("奇校验"));
	m_combo_check.AddString(CString("偶校验"));
	m_combo_check.SetCurSel(0);

	m_combo_stopbit.AddString(CString(std::string(std::to_string(1)).c_str()));
	m_combo_stopbit.AddString(CString(std::string(std::to_string(2)).c_str()));
	m_combo_stopbit.SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}
void CUart::OnBnClickedMainButton()
{
	((void (*)(void*))_data_src_param.GetPtrFuncUpdateData())(_data_src_param.GetPtrFuncUpdateDataParam());
	CString button_text;
	GetDlgItemText(IDC_MAIN_BUTTON, button_text);
	if (button_text == CString("打开串口"))
	{
		SetDlgItemText(IDC_MAIN_BUTTON, CString("关闭串口"));
		((void (*)(void*, bool))_data_src_param.GetPtrFuncEnableControl())(_data_src_param.GetPtrFuncEnableControlParam(), false);
		CString str_temp;
		GetDlgItemTextW(IDC_COMBO_COMS, str_temp);
		std::string coms_str((CW2A)str_temp.GetString());
		char* coms_chars_temp = new char[coms_str.length() - 2];
		for (int i = 3; i < coms_str.length(); ++i)
		{
			coms_chars_temp[i - 3] = coms_str[i];
		}
		coms_chars_temp[coms_str.length() - 3] = '\0';
		int coms = atoi(coms_chars_temp);
		GetDlgItemTextW(IDC_COMBO_BAUDRATE, str_temp);
		int baudRate = _ttoi(str_temp);
		GetDlgItemTextW(IDC_COMBO_DATABIT, str_temp);
		int dataBit = _ttoi(str_temp);
		GetDlgItemTextW(IDC_COMBO_STOPBIT, str_temp);
		int stopBit = _ttoi(str_temp);
		char parity = 'N';
		if (m_combo_check.GetCurSel() != 0)
			parity = 'Y';
		if (!mySerialPort.InitPort(coms, baudRate, parity, dataBit, stopBit))
		{
			::MessageBox(NULL, _T("初始化串口失败！"), _T("提示"), 0);
			SetDlgItemText(IDC_MAIN_BUTTON, CString("打开串口"));
			((void (*)(void*, bool))_data_src_param.GetPtrFuncEnableControl())(_data_src_param.GetPtrFuncEnableControlParam(), true);
			return;
		}

		data_ac_param.forms = this;
		data_ac_param.ptr_serial_port = &mySerialPort;
		data_ac_param.ptr_queue_data = &data_queue_cache;
		data_ac_param.data_queue_ele_default_size = data_queue_cache_ele_default_size;
		process_func_param.forms = this;
		process_func_param.data_process_func_param = _data_src_param.GetPtrFuncDataProThreadParam();
		unsigned int thread_id[2];

		ProcessSoureDataFuncParam* process_data_func_temp = reinterpret_cast<ProcessSoureDataFuncParam*>(_data_src_param.GetPtrFuncDataProThreadParam());
		process_data_func_temp->is_stop = false;
		is_end_data_ac_func_uart = false;

		handle_uart = (HANDLE)_beginthreadex(NULL, 0, DataAcquisitionUart, &data_ac_param, 0, &thread_id[0]);
		handle_data_process = (HANDLE)_beginthreadex(NULL, 0, ProcessFuncUart, &process_func_param, 0, &thread_id[1]);
	}
	else
	{
		is_end_data_ac_func_uart = true;
		ProcessSoureDataFuncParam* process_data_func_temp = reinterpret_cast<ProcessSoureDataFuncParam*>(_data_src_param.GetPtrFuncDataProThreadParam());
		process_data_func_temp->is_stop = true;
		SetDlgItemText(IDC_MAIN_BUTTON, CString("打开串口"));
		((void (*)(void*, bool))_data_src_param.GetPtrFuncEnableControl())(_data_src_param.GetPtrFuncEnableControlParam(), true);
		// 清理数据
		((void (*)(void*))_data_src_param.GetPtrFuncClearData())(_data_src_param.GetPtrFuncClearDataParam());
	}
}

//void CUart::OnBnClickedMainButton()
//{
//	// 更新文件
//	//WriteOperationInfoToFile();
//	mySerialPort.SetReponseType(CSMARTCARCAM);
//
//	CString str;
//	str.Format(_T("打开串口"));
//	CString temp;
//	GetDlgItemTextW(IDC_MAIN_BUTTON, temp);
//	IS_STOP_RECONNECT = false;
//
//	PIC_DATA_SIZE = pic_data_height * pic_data_width;
//
//	if (temp == str)
//	{
//		UpdateParentFormData(update_parent_form_data_param);
//		SetDlgItemTextW(IDC_MAIN_BUTTON, _T("关闭串口"));
//		CString str1;
//		GetDlgItemTextW(IDC_COMBO_COMS, str1);
//		std::string coms_str((CW2A)str1.GetString());
//		char* coms_chars_temp = new char[coms_str.length() - 2];
//		for (int i = 3; i < coms_str.length(); ++i)
//		{
//			coms_chars_temp[i - 3] = coms_str[i];
//		}
//		coms_chars_temp[coms_str.length() - 3] = '\0';
//		int coms = atoi(coms_chars_temp);
//		GetDlgItemTextW(IDC_COMBO_BAUDRATE, str1);
//		int baudRate = _ttoi(str1);
//		GetDlgItemTextW(IDC_COMBO_DATABIT, str1);
//		int dataBit = _ttoi(str1);
//		GetDlgItemTextW(IDC_COMBO_STOPBIT, str1);
//		int stopBit = _ttoi(str1);
//		char parity = 'N';
//		if (m_combo_check.GetCurSel() != 0)
//			parity = 'Y';
//		int nState1 = ((CButton*)GetDlgItem(IDC_CHECK_NOERR))->GetCheck();
//		int nState2 = ((CButton*)GetDlgItem(IDC_CHECK_RECONNECT))->GetCheck();
//		EnableControl(CSmartCam, false);
//		((CWnd*)CSmartCam)->GetDlgItem(IDC_BUTTON2)->EnableWindow(false);
//		((CWnd*)CSmartCam)->GetDlgItem(IDC_BUTTON7)->EnableWindow(false);
//		((CWnd*)CSmartCam)->GetDlgItem(IDC_BUTTON8)->EnableWindow(false);
//
//		if (nState2 == BST_CHECKED)
//		{
//			reconnect_timer.StartTimer(500, std::bind(ReconnectSmartCarCam, this, coms, baudRate, parity, dataBit, stopBit));
//			return;
//		}
//
//		if (mySerialPort.InitPort(coms, baudRate, parity, dataBit, stopBit))
//		{
//			if (!mySerialPort.OpenListenThread())
//			{
//				::MessageBox(NULL, _T("开启监听线程失败!"), _T("提示"), 0);
//				mySerialPort.CloseListenTread();
//				SetDlgItemTextW(IDC_MAIN_BUTTON, _T("打开串口"));
//				EnableControl(CSmartCam, true);
//				((CWnd*)CSmartCam)->GetDlgItem(IDC_BUTTON2)->EnableWindow(true);
//				((CWnd*)CSmartCam)->GetDlgItem(IDC_BUTTON7)->EnableWindow(true);
//				((CWnd*)CSmartCam)->GetDlgItem(IDC_BUTTON8)->EnableWindow(true);
//				return;
//			}
//			CString size_;
//			((CWnd*)CSmartCam)->GetDlgItemTextW(IDC_EDIT_WIDTH, size_);
//			int width_size = _ttoi(size_);
//			((CWnd*)CSmartCam)->GetDlgItemTextW(IDC_EDIT_HEIGTH, size_);
//			int height_size = _ttoi(size_);
//			pic_size = width_size * height_size;
//			pic_height = height_size;
//			pic_width = width_size;
//			reinterpret_cast<RefreshPicFuncParam*>(refresh_pic_func_param)->is_stop = false;
//			refresh_timer.AsyncWait(100, std::bind(Reflesh_Pic, refresh_pic_func_param));
//		}
//		else if (nState1 != BST_CHECKED)
//		{
//			::MessageBox(NULL, _T("初始化串口失败!"), _T("提示"), 0);
//			mySerialPort.CloseListenTread();
//			SetDlgItemTextW(IDC_MAIN_BUTTON, _T("打开串口"));
//			EnableControl(CSmartCam, true);
//			((CWnd*)CSmartCam)->GetDlgItem(IDC_BUTTON2)->EnableWindow(true);
//			((CWnd*)CSmartCam)->GetDlgItem(IDC_BUTTON7)->EnableWindow(true);
//			((CWnd*)CSmartCam)->GetDlgItem(IDC_BUTTON8)->EnableWindow(true);
//		}
//	}
//	else
//	{
//		IS_STOP_RECONNECT = true;
//		IS_FINISH_COLLECT = false;
//		reinterpret_cast<RefreshPicFuncParam*>(refresh_pic_func_param)->is_stop = true;
//		PIC_DATA.clear();
//		reconnect_timer.Expire();
//		mySerialPort.Restart();
//		refresh_timer.Expire();
//		SetDlgItemTextW(IDC_MAIN_BUTTON, _T("打开串口"));
//		EnableControl(CSmartCam, true);
//		((CWnd*)CSmartCam)->GetDlgItem(IDC_BUTTON2)->EnableWindow(true);
//		((CWnd*)CSmartCam)->GetDlgItem(IDC_BUTTON7)->EnableWindow(true);
//		((CWnd*)CSmartCam)->GetDlgItem(IDC_BUTTON8)->EnableWindow(true);
//		ClearParentFormData(clear_data_func_param);
//	}
//}
unsigned __stdcall DataAcquisitionUart(LPVOID lpParam)
{
	UartDataAcParam* ptr = reinterpret_cast<UartDataAcParam*>(lpParam);
	CUart* parent_forms = reinterpret_cast<CUart*>(ptr->forms);
	CSerialPort* pSerialPort = reinterpret_cast<CSerialPort*>(ptr->ptr_serial_port);
	// 线程循环,轮询方式读取串口数据
	while (!is_end_data_ac_func_uart)
	{
		UINT BytesInQue = pSerialPort->GetBytesInCOM();
		/** 如果串口输入缓冲区中无数据,则休息一会再查询 */
		if (BytesInQue == 0)
		{
			Sleep(100);
			continue;
		}
		QueueCacheDataType* temp_str = new QueueCacheDataType(ptr->data_queue_ele_default_size);
		size_t n = pSerialPort->ReadChars(temp_str->GetArray(), ptr->data_queue_ele_default_size - 1);
		temp_str->SetSize(n);
		ptr->ptr_queue_data->push(temp_str);
	}
	return 0;
}
unsigned __stdcall ProcessFuncUart(LPVOID lpParam)
{
	UartProcessFuncParam* ptr = reinterpret_cast<UartProcessFuncParam*>(lpParam);
	CUart* ptr_forms = reinterpret_cast<CUart*>(ptr->forms);
	// 此注释所在函数执行完成与否取决于 DataProcessFunc
	((void (*)(void*))(ptr_forms->_data_src_param.GetPtrFuncDataProThread()))(ptr->data_process_func_param);
	return 0;
}