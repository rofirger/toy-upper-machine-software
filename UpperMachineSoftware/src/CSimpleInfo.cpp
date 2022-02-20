#include "pch.h"
#include "UpperMachineSoftware.h"
#include "CSimpleInfo.h"
#include "afxdialogex.h"
#include "uart.h"
#include <string>
#include <fstream>
#define BUFSIZE    1024
#define SM_SERVERR    289

std::string receive_data;

IMPLEMENT_DYNAMIC(SimpleInfo, CDialogEx)

SimpleInfo::SimpleInfo(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SIMPLEINFO, pParent),
	data_src_param(this, &process_pic_func_param, this, this, EnableSimpleInfoControlForDataSrc, ProcessSimpleInfoData, UpdateSimpleInfoData, ClearSimpleInfoDate, true, _data_queue, raw_data_ele_size),
	forms_cam_uart(data_src_param), forms_cam_net_server(data_src_param), forms_cam_net_client(data_src_param)
{
}

SimpleInfo::~SimpleInfo()
{
}

void SimpleInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO6, m_combo_format);
	DDX_Control(pDX, IDC_SENDFILE, m_check_tofile);
	DDX_Control(pDX, IDC_EDIT1, m_edit_receive);
	DDX_Control(pDX, IDC_TAB_MODE, m_tab_mode);
}

BEGIN_MESSAGE_MAP(SimpleInfo, CDialogEx)
	ON_BN_CLICKED(IDC_SENDFILE, &SimpleInfo::OnBnClickedSendfile)
	ON_BN_CLICKED(IDC_BUTTON1, &SimpleInfo::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON_SEND, &SimpleInfo::OnBnClickedButtonSend)
	ON_WM_SIZE()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_MODE, &SimpleInfo::OnTcnSelchangeTabMode)
END_MESSAGE_MAP()

// SimpleInfo 消息处理程序

void SimpleInfo::OnBnClickedSendfile()
{
	// TODO: 在此添加控件通知处理程序代码
	int nState = ((CButton*)GetDlgItem(IDC_SENDFILE))->GetCheck();
	if (nState == BST_CHECKED)
	{
		//选中
		BOOL isOpen = FALSE;		//是否打开(否则为保存)
		CString defaultDir = L"..//";	//默认打开的文件路径
		CString fileName = L"out.txt";			//默认打开的文件名
		CString filter = L"文件 (*.txt)|*.txt||";	//文件过虑的类型
		CFileDialog openFileDlg(isOpen, defaultDir, fileName, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, filter, NULL);
		openFileDlg.GetOFN().lpstrInitialDir = L"..//out.txt";
		INT_PTR result = openFileDlg.DoModal();
		CString filePath = defaultDir + "\\" + fileName;
		if (result == IDOK)
		{
			filePath = openFileDlg.GetPathName();
			CWnd::SetDlgItemTextW(IDC_PATH, filePath);
			path = filePath;
		}
		else
		{
			CWnd::SetDlgItemTextW(IDC_PATH, _T("C://out.txt"));
			path = "C://out.txt";
		}
		rofirger::add_log(rofirger::LOG_LEVEL_INFO, "select the file{%ls} to save 'simple' bytes", path.GetBuffer());
		path.ReleaseBuffer();
	}
}

BOOL SimpleInfo::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	// 设置tab
	m_tab_mode.InsertItem(0, _T("串口"));
	m_tab_mode.InsertItem(1, _T("TCP服务器"));
	m_tab_mode.InsertItem(2, _T("TCP客户端"));
	forms_cam_uart.Create(IDD_CAM_UART, GetDlgItem(IDC_TAB_MODE));
	forms_cam_net_server.Create(IDD_CAM_NET_SERVER, GetDlgItem(IDC_TAB_MODE));
	forms_cam_net_client.Create(IDD_CAM_NET_CLIENT, GetDlgItem(IDC_TAB_MODE));
	//子窗口大小设置,不设置的话会把tab标题给覆盖掉
	CRect rect_tab;
	m_tab_mode.GetClientRect(rect_tab);
	rect_tab.InflateRect(-1, -28, -2, -2);
	forms_cam_uart.MoveWindow(&rect_tab);     //每一个子窗口大小都要设置
	forms_cam_net_server.MoveWindow(&rect_tab);
	forms_cam_net_client.MoveWindow(&rect_tab);
	//初始状态显示第一个tab页面
	forms_cam_uart.ShowWindow(SW_SHOW);

	m_combo_format.AddString(_T("二进制"));
	m_combo_format.AddString(_T("十六进制"));
	m_combo_format.AddString(_T("字符"));
	m_combo_format.SetCurSel(2);
	return TRUE;
}

void CountBytes(SimpleInfo* ptr)
{
	size_t now_count = 0;
	switch (ptr->m_combo_format.GetCurSel())
	{
	case 0:
	{
		now_count = receive_data.length() / 9;
		break;
	}
	case 1:
	{
		now_count = receive_data.length() / 3;
		break;
	}
	case 2:
	{
		now_count = receive_data.length();
		break;
	}
	default:
		break;
	}
	ptr->lasttime_bytes = now_count;
	CString temp;
	temp.Format(_T("共接收字节: %u 字节"), now_count);
	ptr->SetDlgItemTextW(IDC_receiveinfo, temp);

}

void RefleshBuff(SimpleInfo* ptr)
{
	ptr->SetDlgItemTextW(IDC_EDIT1, CString(receive_data.c_str()));
	ptr->m_edit_receive.LineScroll(ptr->m_edit_receive.GetLineCount() - 1, 0);
	CountBytes(ptr);
}

void SimpleInfo::OnBnClickedButton1()
{
	// 输出到文件
	int sendfile_status = ((CButton*)GetDlgItem(IDC_SENDFILE))->GetCheck();
	if (sendfile_status == BST_CHECKED)
	{
		std::fstream file;
		file.open(path, std::ios::out | std::ios::app);
		if (file.is_open())
		{
			file << receive_data;
			file.close();
		}
		else
		{
			::MessageBox(NULL, _T("打开文件出错!"), _T("提示"), 0);
		}
		rofirger::add_log(rofirger::LOG_LEVEL_INFO, "save the receive data before clearing");
	}

	lasttime_bytes = 0;
	receive_data.clear();
	rofirger::add_log(rofirger::LOG_LEVEL_INFO, "clear the receive data");
	SetDlgItemTextW(IDC_EDIT1, CString(""));
}

void SimpleInfo::OnBnClickedButtonSend()
{
	CString send_str;
	GetDlgItemTextW(IDC_EDIT_SEND, send_str);
	int length_data = send_str.GetLength();
	unsigned char* send_data = new unsigned char[length_data + 1];
	LPWSTR s_s = send_str.GetBuffer();
	for (int i = 0; i < length_data; ++i)
	{
		send_data[i] = s_s[i];
	}
	send_str.ReleaseBuffer();
	send_data[length_data] = '\0';
	mySerialPort.WriteData(send_data, length_data);
	rofirger::add_log(rofirger::LOG_LEVEL_INFO, "send {%s} with %d bytes to serial port", send_data, length_data);
}

void SimpleInfo::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	// tab
	CRect rect_tab_mode;
	CWnd* pWnd_rect_tab_mode = GetDlgItem(IDC_TAB_MODE);
	if (pWnd_rect_tab_mode)
	{
		pWnd_rect_tab_mode->GetWindowRect(&rect_tab_mode);
		ScreenToClient(&rect_tab_mode);
	}

	// 串口配置
	CRect client_rect;
	GetClientRect(&client_rect);
	// 辅助模块
	CRect rect_static_assit;
	CWnd* pWd_static_assit = GetDlgItem(IDC_STATIC_ASSIT);
	// 发送接收
	CRect rect_edit_receive;
	CWnd* pWnd_edit_receive = GetDlgItem(IDC_EDIT1);
	CRect rect_edit_send;
	CWnd* pWnd_edit_send = GetDlgItem(IDC_EDIT_SEND);

	CWnd* pWnd_button = GetDlgItem(IDC_MAIN_BUTTON);
	CWnd* pWnd_static_format = GetDlgItem(IDC_STATIC_FORMAT);
	CWnd* pWnd_format = GetDlgItem(IDC_COMBO6);
	CWnd* pWnd_sendfile = GetDlgItem(IDC_SENDFILE);
	CWnd* pWnd_path = GetDlgItem(IDC_PATH);
	CWnd* pWnd_clear = GetDlgItem(IDC_BUTTON1);
	CWnd* pWnd_static_receive = GetDlgItem(IDC_STATIC_RECEIVE);
	CWnd* pWnd_static_send = GetDlgItem(IDC_STATIC_SEND);
	CWnd* pWnd_receiveinfo = GetDlgItem(IDC_receiveinfo);
	CWnd* pWnd_static_6 = GetDlgItem(IDC_STATIC);
	CWnd* pWnd_button_send = GetDlgItem(IDC_BUTTON_SEND);
	if (pWnd_button && pWnd_static_format && pWnd_format && pWnd_sendfile && pWnd_path && pWnd_clear)
	{
		pWnd_button->GetWindowRect(&rect_button);
		ScreenToClient(&rect_button);
		pWnd_static_format->GetWindowRect(&rect_s_format);
		ScreenToClient(&rect_s_format);
		pWnd_format->GetWindowRect(&rect_format);
		ScreenToClient(&rect_format);
		pWnd_sendfile->GetWindowRect(&rect_sendfile);
		ScreenToClient(&rect_sendfile);
		pWnd_path->GetWindowRect(&rect_path);
		ScreenToClient(&rect_path);
		pWnd_clear->GetWindowRect(&rect_clear);
		ScreenToClient(&rect_clear);
		pWnd_edit_receive->GetWindowRect(&rect_edit_receive);
		ScreenToClient(&rect_edit_receive);
		pWnd_edit_send->GetWindowRect(&rect_edit_send);
		ScreenToClient(&rect_edit_send);
		pWnd_receiveinfo->GetWindowRect(&rect_receiveinfo);
		ScreenToClient(&rect_receiveinfo);
		pWnd_static_6->GetWindowRect(&rect_s_6);
		ScreenToClient(&rect_s_6);
		pWnd_button_send->GetWindowRect(&rect_button_send);
		ScreenToClient(&rect_button_send);
	}

	if (pWnd_rect_tab_mode && pWnd_static_format && pWnd_format && pWnd_sendfile && pWnd_path && pWnd_clear)
	{
		pWd_static_assit->SetWindowPos(NULL, rect_tab_mode.left, rect_tab_mode.bottom + 13, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		pWd_static_assit->GetWindowRect(&rect_static_assit);
		ScreenToClient(&rect_static_assit);

		pWnd_static_format->SetWindowPos(NULL, rect_static_assit.left + 20, rect_static_assit.top + 40, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		pWnd_static_format->GetWindowRect(&rect_s_format);
		ScreenToClient(&rect_s_format);

		pWnd_format->SetWindowPos(NULL, rect_static_assit.right - rect_format.Width() - 20, rect_static_assit.top - (rect_format.Height() - rect_s_format.Height()) / 2 + 40,
			0, 0, SWP_NOZORDER | SWP_NOSIZE);
		pWnd_format->GetWindowRect(&rect_format);
		ScreenToClient(&rect_format);

		pWnd_sendfile->SetWindowPos(NULL, rect_s_format.left, rect_s_format.bottom + 20, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		pWnd_sendfile->GetWindowRect(&rect_sendfile);
		ScreenToClient(&rect_sendfile);

		pWnd_path->SetWindowPos(NULL, rect_s_format.left, rect_sendfile.bottom + 20, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		pWnd_path->GetWindowRect(&rect_path);
		ScreenToClient(&rect_path);

		pWnd_clear->SetWindowPos(NULL, rect_s_format.left, rect_path.bottom + 20, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		pWnd_path->GetWindowRect(&rect_clear);
		ScreenToClient(&rect_clear);

		// 改变右边区域
		pWnd_static_receive->SetWindowPos(NULL, rect_tab_mode.right + 20, client_rect.top + 30, client_rect.right - rect_tab_mode.right - 60, client_rect.bottom / 2 - 15, SWP_NOZORDER);
		pWnd_static_receive->GetWindowRect(&rect_static_receive);
		ScreenToClient(&rect_static_receive);

		pWnd_static_send->SetWindowPos(NULL, rect_tab_mode.right + 20, rect_static_receive.bottom + 20, client_rect.right - rect_tab_mode.right - 60, client_rect.bottom / 2 - 80, SWP_NOZORDER);
		pWnd_static_send->GetWindowRect(&rect_static_send);
		ScreenToClient(&rect_static_send);

		pWnd_receiveinfo->SetWindowPos(NULL, rect_static_receive.left + 20, rect_static_receive.top + rect_receiveinfo.Height() + 10, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		pWnd_receiveinfo->GetWindowRect(&rect_receiveinfo);
		ScreenToClient(&rect_receiveinfo);

		pWnd_edit_receive->SetWindowPos(NULL,
			rect_static_receive.left + 20, rect_receiveinfo.bottom + 10,
			rect_static_receive.Width() - 40, rect_static_receive.bottom - (rect_receiveinfo.bottom + 10) - 10,
			SWP_NOZORDER);
		pWnd_edit_receive->GetWindowRect(&rect_edit_receive);
		ScreenToClient(&rect_edit_receive);

		pWnd_static_6->SetWindowPos(NULL, rect_edit_receive.right - rect_s_6.Width(), rect_receiveinfo.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		pWnd_static_6->GetWindowRect(&rect_s_6);
		ScreenToClient(&rect_s_6);

		pWnd_edit_send->SetWindowPos(NULL,
			rect_static_send.left + 20, rect_static_send.top + 30,
			rect_static_send.Width() - 40, rect_static_send.Height() - 80,
			SWP_NOZORDER);
		pWnd_edit_send->GetWindowRect(&rect_edit_send);
		ScreenToClient(&rect_edit_send);

		pWnd_button_send->SetWindowPos(NULL,
			rect_edit_send.left + (rect_edit_send.Width() - rect_button_send.Width()) / 2,
			rect_static_send.bottom - rect_button_send.Height() - 5,
			0, 0,
			SWP_NOSIZE | SWP_NOZORDER);
		pWnd_button_send->GetWindowRect(&rect_button_send);
		ScreenToClient(&rect_button_send);
		// 位置发生冲突时使控件不可见
		GetDlgItem(IDC_STATIC)->ShowWindow(rect_receiveinfo.left <= rect_s_6.left);
	}

	// 以上串口配置区域位置调整
}
string UcharToHex(unsigned char num)
{
	string str;
	unsigned char temp = num / 16;
	int left = num % 16;
	if (temp > 0)
		str += UcharToHex(temp);
	if (left < 10)
		str += (left + '0');
	else
		str += ('A' + left - 10);
	return str;
}
string UcharsToHex(unsigned char* num, size_t s_)
{
	string str;
	for (size_t i = 0; i < s_; ++i)
	{
		unsigned char temp = num[i] / 16;
		int left = num[i] % 16;
		if (temp > 0)
			str += UcharToHex(temp);
		if (left < 10)
			str += (left + '0');
		else
			str += ('A' + left - 10);
		str += ' ';
	}

	return str;
}
string UcharsToBinary(unsigned char* num, size_t s_)
{
	string str;
	for (size_t i = 0; i < s_; ++i)
	{
		unsigned char bit = 0x80;
		for (int i = 0; i < 8; ++i)
		{
			if (bit & num[i])
				str += '1';
			else
				str += '0';
			num[i] = num[i] << 1;
		}
		str += ' ';
	}
	return str;
}
void EnableSimpleInfoControlForDataSrc(void* ptr_param_simple_info, bool true_or_false)
{
	SimpleInfo* ptr = reinterpret_cast<SimpleInfo*>(ptr_param_simple_info);
	ptr->GetDlgItem(IDC_SENDFILE)->EnableWindow(true_or_false);
	ptr->GetDlgItem(IDC_PATH)->EnableWindow(true_or_false);
}
void ProcessSimpleInfoData(void* ptr_param)
{
	SimpleInfo* ptr = reinterpret_cast<SimpleInfo*>(reinterpret_cast<ProcessSoureDataFuncParam*>(ptr_param)->form);
	bool& is_stop_this_func = reinterpret_cast<ProcessSoureDataFuncParam*>(ptr_param)->is_stop;
	SimpleinfoFormat format_ = (SimpleinfoFormat)ptr->m_combo_format.GetCurSel();

	while (!is_stop_this_func)
	{
		while (ptr->_data_queue.empty())
		{
			Sleep(100);
			if (is_stop_this_func)
			{
				return;
			}
		}
		unsigned char* ptr_raw_data_array = ptr->_data_queue.front()->GetArray();
		size_t s_ = ptr->_data_queue.front()->GetSize();
		unsigned char* temp_new_data = new unsigned char[s_ + 1];
		for (size_t i = 0; i < s_; ++i)
		{
			temp_new_data[i] = ptr_raw_data_array[i];
		}
		temp_new_data[s_] = '\0';
		switch (format_)
		{
		case BD:
		{
			receive_data += std::string(UcharsToBinary(temp_new_data, s_));
			break;
		}
		case HD:
		{
			receive_data += std::string(UcharsToHex(temp_new_data, s_));
			break;
		}
		case CD:
		{
			receive_data += std::string((char*)temp_new_data);
			break;
		}
		default:
			break;
		}
		RefleshBuff(ptr);
		if (!ptr->_data_queue.empty())
		{
			ptr->_data_queue.front()->Release();
			delete ptr->_data_queue.front();
			ptr->_data_queue.pop();
		}
		delete[]temp_new_data;
	}

	// 输出到文件
	int sendfile_status = ((CButton*)ptr->GetDlgItem(IDC_SENDFILE))->GetCheck();
	if (sendfile_status == BST_CHECKED)
	{
		std::fstream file;
		file.open(ptr->path, std::ios::out | std::ios::app);
		if (file.is_open())
		{
			file << receive_data;
			file.close();
		}
		else
		{
			::MessageBox(NULL, _T("打开文件出错!"), _T("提示"), 0);
		}
	}
}
void UpdateSimpleInfoData(void* ptr_param)
{}
void ClearSimpleInfoDate(void* ptr_param)
{
	SimpleInfo* ptr = reinterpret_cast<SimpleInfo*>(ptr_param);
	while (!ptr->_data_queue.empty())
	{
		ptr->_data_queue.front()->Release();
		delete ptr->_data_queue.front();
		ptr->_data_queue.pop();
	}
}

void SimpleInfo::OnTcnSelchangeTabMode(NMHDR* pNMHDR, LRESULT* pResult)
{
	int num = m_tab_mode.GetCurSel();//获取点击了哪一个页面
	switch (num)
	{
	case 0:
		forms_cam_uart.ShowWindow(SW_SHOW);
		forms_cam_net_server.ShowWindow(SW_HIDE);
		forms_cam_net_client.ShowWindow(SW_HIDE);
		break;
	case 1:
		forms_cam_uart.ShowWindow(SW_HIDE);
		forms_cam_net_server.ShowWindow(SW_SHOW);
		forms_cam_net_client.ShowWindow(SW_HIDE);
		break;
	case 2:
		forms_cam_uart.ShowWindow(SW_HIDE);
		forms_cam_net_server.ShowWindow(SW_HIDE);
		forms_cam_net_client.ShowWindow(SW_SHOW);
		break;
	}
	*pResult = 0;
}
