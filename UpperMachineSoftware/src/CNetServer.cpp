#include "pch.h"
#include "UpperMachineSoftware.h"
#include "CNetServer.h"
#include "afxdialogex.h"
bool is_end_data_ac_func_net_server = false;
IMPLEMENT_DYNAMIC(CNetServer, CDialogEx)
#pragma   warning(disable:4996)
CNetServer::CNetServer(DataSrcParam& data_src_param) :_data_src_param(data_src_param),
data_queue_cache(data_src_param.GetDataQueue()),
data_queue_cache_ele_default_size(data_src_param.GetDataQueueDefaultEleSize())
{
	handle_client = nullptr;
	handle_data_process = nullptr;
}
CNetServer::~CNetServer() {}

void CNetServer::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IPADDRESS_LOCAL, m_ip);
	DDX_Control(pDX, IDC_EDIT_PORT, m_port);
}

BEGIN_MESSAGE_MAP(CNetServer, CDialogEx)
	ON_BN_CLICKED(IDC_MAIN_BUTTON, &CNetServer::OnBnClickedMainButton)
	ON_EN_KILLFOCUS(IDC_EDIT_PORT, &CNetServer::OnEnKillfocusEditPort)
	ON_WM_SIZE()
END_MESSAGE_MAP()

BOOL CNetServer::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	SetDlgItemText(IDC_EDIT_PORT, CString("80"));
	SetDlgItemText(IDC_EDIT_MAX_CONTACT, CString("1"));
	// 禁止用户输入最大连接数
	GetDlgItem(IDC_EDIT_MAX_CONTACT)->EnableWindow(false);
	port = 80;
	max_contact = 1;
	WORD wVersionRequested;
	WSADATA wsaData;
	char name[255];
	CString c_ip;
	PHOSTENT host_info;
	wVersionRequested = MAKEWORD(2, 0);

	if (WSAStartup(wVersionRequested, &wsaData) == 0)
	{
		if (gethostname(name, sizeof(name)) == 0)
		{
			if ((host_info = gethostbyname(name)) != NULL)
			{
				c_ip = inet_ntoa(*(struct in_addr*)host_info->h_addr_list[0]);
			}
		}
		WSACleanup();
	}
	m_ip.SetWindowText(c_ip);
	ip = CT2A(c_ip.GetBuffer());
	c_ip.ReleaseBuffer();
	GetDlgItem(IDC_IPADDRESS_LOCAL)->EnableWindow(false);
	return TRUE;
}


void CNetServer::OnBnClickedMainButton()
{
	((void(*)(void*))(_data_src_param.GetPtrFuncUpdateData()))(_data_src_param.GetPtrFuncUpdateDataParam());
	CString button_text;
	GetDlgItemText(IDC_MAIN_BUTTON, button_text);
	if (button_text == CString("监听"))
	{
		SetDlgItemText(IDC_MAIN_BUTTON, CString("停止"));
		((void(*)(void*, bool))(_data_src_param.GetPtrFuncEnableControl()))(_data_src_param.GetPtrFuncEnableControlParam(), false);
		if (!sock.Create(port, SOCK_STREAM, (char*)ip.c_str()))
		{
			CString temp_fail;
			temp_fail.Format(_T("创建失败:%d"), GetLastError());
			MessageBox(temp_fail, _T("提示"), 0);
			SetDlgItemText(IDC_MAIN_BUTTON, _T("监听"));
			((void(*)(void*, bool))(_data_src_param.GetPtrFuncEnableControl()))(_data_src_param.GetPtrFuncEnableControlParam(), true);
			return;
		}
		//监听
		sock.Listen(max_contact);
		CDialog dlg;
		dlg.Create(IDD_DIALOG_NET_SERVER_WAIT);
		dlg.CenterWindow(this);
		dlg.ShowWindow(SW_SHOWNOACTIVATE);
		dlg.UpdateWindow();
		sock.Accept(socka, sIp, &nPort);
		dlg.EndDialog(0);
		data_ac_param.forms = this;
		data_ac_param.ptr_sock = &socka;
		data_ac_param.ip = ip;
		data_ac_param.port = port;
		data_ac_param.ptr_queue_data = &data_queue_cache;
		data_ac_param.data_queue_ele_default_size = data_queue_cache_ele_default_size;
		process_func_param.forms = this;
		process_func_param.data_process_func_param = _data_src_param.GetPtrFuncDataProThreadParam();
		unsigned int thread_id[2];

		ProcessSoureDataFuncParam* process_pic_func_temp = reinterpret_cast<ProcessSoureDataFuncParam*>(_data_src_param.GetPtrFuncDataProThreadParam());
		process_pic_func_temp->is_stop = false;
		is_end_data_ac_func_net_server = false;

		handle_client = (HANDLE)_beginthreadex(NULL, 0, DataAcquisitionNetServer, &data_ac_param, 0, &thread_id[0]);
		handle_data_process = (HANDLE)_beginthreadex(NULL, 0, ProcessFuncNetServer, &process_func_param, 0, &thread_id[1]);

	}
	else
	{
		sock.Close();
		is_end_data_ac_func_net_server = true;
		ProcessSoureDataFuncParam* process_pic_func_temp = reinterpret_cast<ProcessSoureDataFuncParam*>(_data_src_param.GetPtrFuncDataProThreadParam());
		process_pic_func_temp->is_stop = true;
		SetDlgItemText(IDC_MAIN_BUTTON, CString("监听"));
		((void(*)(void*, bool))(_data_src_param.GetPtrFuncEnableControl()))(_data_src_param.GetPtrFuncEnableControlParam(), true);
		// 清理数据
		((void(*)(void*))(_data_src_param.GetPtrFuncClearData()))(_data_src_param.GetPtrFuncClearDataParam());
	}
}


void CNetServer::OnEnKillfocusEditPort()
{
	UpdateData(true);
	CString user_port_str;
	GetDlgItemText(IDC_EDIT_PORT, user_port_str);
	long long user_port = _ttoi(user_port_str);
	if (user_port < 0 || user_port>65535)
	{
		MessageBox(_T("端口号错误! 端口号范围为[0, 65535]!"), _T("错误"), 0);
		SetDlgItemText(IDC_EDIT_PORT, CString("80"));
	}
	port = user_port;
	UpdateData(false);
}


void CNetServer::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	CRect client_rect;
	GetClientRect(&client_rect);
	CWnd* pWnd_static_boarder = GetDlgItem(IDC_STATIC_BOARDER);
	CWnd* pWnd_static_ip = GetDlgItem(IDC_STATIC_IP);
	CWnd* pWnd_ipaddress_ip = GetDlgItem(IDC_IPADDRESS_LOCAL);
	CWnd* pWnd_static_port = GetDlgItem(IDC_STATIC_PORT);
	CWnd* pWnd_static_max_contact = GetDlgItem(IDC_STATIC_MAX_CONTACT);
	CWnd* pWnd_edit_port = GetDlgItem(IDC_EDIT_PORT);
	CWnd* pWnd_edit_max_contact = GetDlgItem(IDC_EDIT_MAX_CONTACT);
	CWnd* pWnd_button_main = GetDlgItem(IDC_MAIN_BUTTON);
	if (pWnd_static_boarder && pWnd_static_ip && pWnd_ipaddress_ip && pWnd_static_port
		&& pWnd_static_max_contact && pWnd_edit_port && pWnd_edit_max_contact && pWnd_button_main)
	{
		pWnd_static_boarder->GetWindowRect(&rect_static_boarder);
		ScreenToClient(&rect_static_boarder);
		pWnd_static_ip->GetWindowRect(&rect_static_ip);
		ScreenToClient(&rect_static_ip);
		pWnd_ipaddress_ip->GetWindowRect(&rect_ipaddress_ip);
		ScreenToClient(&rect_ipaddress_ip);
		pWnd_static_port->GetWindowRect(&rect_static_port);
		ScreenToClient(&rect_static_port);
		pWnd_static_max_contact->GetWindowRect(&rect_static_max_contact);
		ScreenToClient(&rect_static_max_contact);
		pWnd_edit_port->GetWindowRect(&rect_edit_port);
		ScreenToClient(&rect_edit_port);
		pWnd_edit_max_contact->GetWindowRect(&rect_edit_max_contact);
		ScreenToClient(&rect_edit_max_contact);
		pWnd_button_main->GetWindowRect(&rect_button_main);
		ScreenToClient(&rect_button_main);

		pWnd_static_boarder->SetWindowPos(NULL, client_rect.left + 20, client_rect.top + 20, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
		pWnd_static_boarder->GetWindowRect(&rect_static_boarder);
		ScreenToClient(&rect_static_boarder);

		pWnd_static_ip->SetWindowPos(NULL, rect_static_boarder.left + 20, rect_static_boarder.top + 30, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		pWnd_static_ip->GetWindowRect(&rect_static_ip);
		ScreenToClient(&rect_static_ip);

		pWnd_ipaddress_ip->SetWindowPos(NULL, rect_static_ip.left, rect_static_ip.bottom + 15, rect_static_boarder.Width() - 40, rect_ipaddress_ip.Height(), SWP_NOZORDER);
		pWnd_ipaddress_ip->GetWindowRect(&rect_ipaddress_ip);
		ScreenToClient(&rect_ipaddress_ip);

		pWnd_static_port->SetWindowPos(NULL, rect_ipaddress_ip.left, rect_ipaddress_ip.bottom + 30, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		pWnd_static_port->GetWindowRect(&rect_static_port);
		ScreenToClient(&rect_static_port);

		pWnd_static_max_contact->SetWindowPos(NULL, rect_static_port.left, rect_static_port.bottom + 30, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		pWnd_static_max_contact->GetWindowRect(&rect_static_max_contact);
		ScreenToClient(&rect_static_max_contact);

		pWnd_edit_port->SetWindowPos(NULL, rect_ipaddress_ip.right - rect_edit_port.Width(), rect_static_port.top - (rect_edit_port.Height() - rect_static_port.Height()) / 2, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		pWnd_edit_port->GetWindowRect(&rect_edit_port);
		ScreenToClient(&rect_edit_port);

		pWnd_edit_max_contact->SetWindowPos(NULL, rect_edit_port.left, rect_static_max_contact.top - (rect_edit_max_contact.Height() - rect_static_max_contact.Height()) / 2, rect_edit_port.Width(), rect_edit_port.Height(), SWP_NOZORDER);
		pWnd_edit_max_contact->GetWindowRect(&rect_edit_max_contact);
		ScreenToClient(&rect_edit_max_contact);

		pWnd_button_main->SetWindowPos(NULL, rect_static_boarder.left + (rect_static_boarder.Width() - rect_button_main.Width()) / 2, rect_static_boarder.bottom - rect_button_main.Height() - 20, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		pWnd_button_main->GetWindowRect(&rect_button_main);
		ScreenToClient(&rect_button_main);
	}
}
unsigned __stdcall DataAcquisitionNetServer(LPVOID lpParam)
{
	NetServerDataAcParam* ptr = reinterpret_cast<NetServerDataAcParam*>(lpParam);
	CNetServer* parent_forms = reinterpret_cast<CNetServer*>(ptr->forms);
	while (!is_end_data_ac_func_net_server)
	{
		QueueCacheDataType* temp_str = new QueueCacheDataType(ptr->data_queue_ele_default_size);
		int n = ptr->ptr_sock->Receive(temp_str->GetArray(), ptr->data_queue_ele_default_size - 1);
		if (n <= 0)
		{
			if (temp_str->GetSize() != 0)
			{
				temp_str->Release();
				delete temp_str;
			}
			temp_str->~QueueCacheDataType();
			MessageBox(NULL, _T("连接已被取消!"), _T("提示"), 0);
			parent_forms->SetDlgItemText(IDC_MAIN_BUTTON, CString("连接"));
			//parent_forms->EnableControl(parent_forms->CSmartCam, true);
			((void(*)(void*, bool))(parent_forms->_data_src_param.GetPtrFuncEnableControl()))(parent_forms->_data_src_param.GetPtrFuncEnableControlParam(), true);
			return 0;
		}
		temp_str->SetSize(n);
		ptr->ptr_queue_data->push(temp_str);
	}
	ptr->ptr_sock->Close();
	return 0;
}
unsigned __stdcall ProcessFuncNetServer(LPVOID lpParam)
{
	NetProcessFuncParam* ptr = reinterpret_cast<NetProcessFuncParam*>(lpParam);
	CNetServer* ptr_forms = reinterpret_cast<CNetServer*>(ptr->forms);
	// 此注释所在函数执行完成与否取决于 DataProcessFunc
	((void(*)(void*))(ptr_forms->_data_src_param.GetPtrFuncDataProThread()))(ptr->data_process_func_param);
	//ptr_forms->DataProcessFunc(ptr->data_process_func_param);
	return 0;
}