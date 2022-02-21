#include "pch.h"
#include "framework.h"
#include "UpperMachineSoftware.h"
#include "UpperMachineSoftwareDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern double* _x;
CString app_open_file_path;

CUpperMachineSoftwareDlg::CUpperMachineSoftwareDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_UPPERMACHINE_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CUpperMachineSoftwareDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, m_tab);
}

BEGIN_MESSAGE_MAP(CUpperMachineSoftwareDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CUpperMachineSoftwareDlg::OnTcnSelchangeTab1)
	ON_WM_CLOSE()
	ON_WM_DROPFILES()
	ON_WM_SIZE()
END_MESSAGE_MAP()

void Check(CUpperMachineSoftwareDlg* dlg)
{
	dlg->CheckVer();
}
BOOL CUpperMachineSoftwareDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	m_tab.InsertItem(0, _T("信息调试"));
	m_tab.InsertItem(1, _T("图像调试"));
	m_tab.InsertItem(2, _T("实时数据变化波形化"));

	uart_simple.Create(IDD_SIMPLEINFO, GetDlgItem(IDC_TAB1));
	uart_pic.Create(IDD_IMGPROCESS, GetDlgItem(IDC_TAB1));
	uart_rtwaveform.Create(IDD_PID, GetDlgItem(IDC_TAB1));

	//子窗口大小设置,不设置的话会把tab标题给覆盖掉
	CRect rect;
	m_tab.GetClientRect(rect);
	rect.InflateRect(-1, -30, -2, -2);
	uart_simple.MoveWindow(&rect);     //每一个子窗口大小都要设置
	uart_pic.MoveWindow(&rect);
	uart_rtwaveform.MoveWindow(&rect);

	//初始状态显示第一个tab页面
	uart_simple.ShowWindow(SW_SHOW);

	// 以该软件方式打开文件
	if (!app_open_file_path.IsEmpty())
	{
		uart_pic.is_close_look = false;
		uart_pic.drop_files.push_back(app_open_file_path);
		uart_pic.file_look = new CFileLook(uart_pic.drop_files, uart_pic.is_close_look);
		uart_pic.file_look->Create(IDD_FILE_LOOK);
		uart_pic.file_look->ShowWindow(SW_SHOWNORMAL);
		rofirger::add_log(rofirger::LOG_LEVEL_INFO, "create FILE_PIC_VIEW window with %d pics successfully", uart_pic.drop_files.size());
	}
	_timer_for_check_ver.AsyncWait(1000, std::bind(Check, this));
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}
void CUpperMachineSoftwareDlg::CheckVer()
{
	rofirger::add_log(rofirger::LOG_LEVEL_INFO, "enter into the check_ver func successfully");
	OnSize(1, 100, 100);
	// 下载版本信息
	FindExePath();
	CString strFileName_ini, strControlName;
	//从配置文件读入当前版本号
	strFileName_ini = exe_path + _T("\\ver\\VerInfo.ini");
	LPWSTR str_file_name_ini = strFileName_ini.GetBuffer();
	struct stat stat_info;
	if (_waccess(str_file_name_ini, 0) == -1 || _waccess(str_file_name_ini, 04) == -1)
	{
		rofirger::add_log(rofirger::LOG_LEVEL_FATAL, "file {%ls} doesn't can be accessed", str_file_name_ini);
		::MessageBox(NULL, _T("VerInfo.ini文件有误, 请联系作者QQ: 842474373. 或访问文件直链 http://rofirger.xyz/smcd_update.ini"), _T("提醒"), 0);
	}
	//获取当前配置文件的版本号
	::GetPrivateProfileString(_T("MAINVER"), _T("mainver"), NULL, strControlName.GetBuffer(128), 128, strFileName_ini);
	strControlName.ReleaseBuffer();
	std::string old_ver_str(CW2A(strControlName.GetString()));
	if (old_ver_str.empty())
	{
		rofirger::add_log(rofirger::LOG_LEVEL_WARNING, "file {%ls} doesn't have ver_num", str_file_name_ini);
	}
	strFileName_ini.ReleaseBuffer();
	double flCurVer = atof((char*)old_ver_str.c_str());			//保存当前版本号
	strControlName.ReleaseBuffer();

	CString strUrl = _T("http://rofirger.xyz/smcd_update.ini");
	CString   strFolderPath = exe_path + _T("\\ver");
	if (!PathIsDirectory(strFolderPath))
	{
		CreateDirectory(strFolderPath, NULL);
		rofirger::add_log(rofirger::LOG_LEVEL_FATAL, "create new folder{%ls}. That means this computer is missing VerInfo.ini!", strFolderPath.GetBuffer());
		strFolderPath.ReleaseBuffer();
	}
	CString update_ini = exe_path + _T("\\ver\\smcd_update.ini");
	DownloadFile(strUrl, update_ini);

	//从下载来的配置文件读入最新版本号
	strFileName_ini.Replace(_T("VerInfo.ini"), _T("smcd_update.ini"));
	::GetPrivateProfileString(_T("MAINVER"), _T("mainver"), NULL, strControlName.GetBuffer(128), 128, strFileName_ini);
	strControlName.ReleaseBuffer();
	std::string new_ver_str(CW2A(strControlName.GetString()));
	double flNewVer = atof((char*)new_ver_str.c_str());
	rofirger::add_log(rofirger::LOG_LEVEL_INFO, "the current version of this software is %lf, the latest version of this software is %lf", flCurVer, flNewVer);
	//对比版本号,如果大于本地版本就启动更新程序进行更新
	if (flNewVer > flCurVer)
	{
		//启动update.exe
		CString strPath = exe_path + "\\update.exe";
		std::string update_exe_path(CW2A(strPath.GetString()));
		HINSTANCE hID = ShellExecuteA(NULL, "open", update_exe_path.c_str(), NULL, NULL, SW_SHOWNORMAL);
		if ((int)hID <= 32)
		{
			rofirger::add_log(rofirger::LOG_LEVEL_ERROR, "This computer is missing update.exe");
			::MessageBox(NULL, _T("程序目录下缺少升级程序update.exe, 请联系作者QQ: 842474373. 或访问文件直链 http://rofirger.xyz/update.exe"), _T("提示"), 0);
		}
		rofirger::add_log(rofirger::LOG_LEVEL_INFO, "exit this process for updating");
		//关闭本程序
		ExitProcess(0);
	}
	else
	{
		::DeleteFile(strFileName_ini);						//删除下载来的配置文件
	}
}
void CUpperMachineSoftwareDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CUpperMachineSoftwareDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CUpperMachineSoftwareDlg::OnTcnSelchangeTab1(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	int num = m_tab.GetCurSel();//获取点击了哪一个页面
	switch (num)
	{
	case 0:
		uart_simple.ShowWindow(SW_SHOW);
		uart_pic.ShowWindow(SW_HIDE);
		uart_rtwaveform.ShowWindow(SW_HIDE);
		break;
	case 1:
		uart_simple.ShowWindow(SW_HIDE);
		uart_pic.ShowWindow(SW_SHOW);
		uart_rtwaveform.ShowWindow(SW_HIDE);
		break;
	case 2:
		uart_simple.ShowWindow(SW_HIDE);
		uart_pic.ShowWindow(SW_HIDE);
		uart_rtwaveform.ShowWindow(SW_SHOW);
		break;
	}
	rofirger::add_log(rofirger::LOG_LEVEL_INFO, "select the %d tab in the main window", num);
	*pResult = 0;
}

void CUpperMachineSoftwareDlg::OnClose()
{
	delete[]_x;
	uart_pic.WriteOperationInfoToFile();
	CDialogEx::OnClose();
	_CrtDumpMemoryLeaks();
	rofirger::add_log(rofirger::LOG_LEVEL_INFO, "CLOSING THIS SOFTWARE");
	// 关闭日志
	rofirger::Log::GetInstance()->StopLog();
}

void CUpperMachineSoftwareDlg::OnDropFiles(HDROP hDropInfo)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	int  nFileCount = ::DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 256);   //获取拖入的文件数量
	rofirger::add_log(rofirger::LOG_LEVEL_INFO, "drop %d files", nFileCount);
	for (int i = 0; i < nFileCount; i++)
	{
		CString m_file;// 完整路径
		UINT nChars = ::DragQueryFile(hDropInfo, i, m_file.GetBuffer(256), 256);   // 获取拖入的第i个文件的文件名
		m_file.ReleaseBuffer();
		uart_pic.drop_files.push_back(m_file);
		rofirger::add_log(rofirger::LOG_LEVEL_INFO, "dropped file:%ls", m_file.GetBuffer());
		m_file.ReleaseBuffer();
	}
	::DragFinish(hDropInfo);
	if (!uart_pic.is_close_look)
	{
		uart_pic.file_look->Reflesh();
		rofirger::add_log(rofirger::LOG_LEVEL_INFO, "the *.sfp file view windown is running while dropping files");
	}
	else
	{
		rofirger::add_log(rofirger::LOG_LEVEL_INFO, "the *.sfp file view windown isn't running while dropping files");
		if (uart_pic.file_look != NULL)
		{
			delete uart_pic.file_look;
		}
		uart_pic.file_look = new CFileLook(uart_pic.drop_files, uart_pic.is_close_look);
		uart_pic.file_look->Create(IDD_FILE_LOOK);
		uart_pic.file_look->ShowWindow(SW_SHOWNORMAL);
	}
	CDialogEx::OnDropFiles(hDropInfo);
}
bool is_size = false;
void CUpperMachineSoftwareDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	if (is_size)
	{
		CRect rect_pid;
		CRect rect_simple;
		CRect rect_car;
		uart_rtwaveform.GetWindowRect(&rect_pid);
		uart_pic.GetWindowRect(&rect_car);
		uart_simple.GetWindowRect(&rect_simple);

		CRect rect_this;
		GetWindowRect(&rect_this);
		CRect rect_this_client;
		GetClientRect(&rect_this_client);
		CWnd* pWnd_tab = GetDlgItem(IDC_TAB1);
		if (pWnd_tab)
		{
			pWnd_tab->SetWindowPos(NULL, 0, 0, rect_this_client.Width(), rect_this_client.Height(), SWP_NOMOVE | SWP_NOZORDER);
		}

		uart_rtwaveform.MoveWindow(CRect(1, 30, rect_this_client.Width(), rect_this_client.Height()));
		uart_pic.MoveWindow(CRect(1, 30, rect_this_client.Width(), rect_this_client.Height()));
		uart_simple.MoveWindow(CRect(1, 30, rect_this_client.Width(), rect_this_client.Height()));
	}

	is_size = true;
}
// 只需实现OnProgress方法，类的实现：
CBindCallback::CBindCallback() {}

CBindCallback::~CBindCallback() {}

//仅实现OnProgress成员即可

HRESULT CBindCallback::OnProgress(ULONG ulProgress,
	ULONG ulProgressMax,
	ULONG ulStatusCode,
	LPCWSTR wszStatusText)
{
	return S_OK;
}
void CUpperMachineSoftwareDlg::DownloadFile(CString szUrl, CString szPath)
{
	CBindCallback cbc;
	//在url后添加随机数，防止从IE缓存中读取。url后加随机数不会影响下载的。
	//如果想要从缓存中提取那么就把下面的注释掉
	CString szRand;
	szRand.Format(_T("?skq=%d"), GetTickCount());
	szUrl += szRand;
	rofirger::add_log(rofirger::LOG_LEVEL_INFO, "download {%ls} from {%ls}", szPath.GetBuffer(), szUrl.GetBuffer());
	szUrl.ReleaseBuffer(); szPath.ReleaseBuffer();
	URLDownloadToFile(NULL, szUrl, szPath, 0, &cbc);
	rofirger::add_log(rofirger::LOG_LEVEL_INFO, "download the *.ini file successfully");
}
void CUpperMachineSoftwareDlg::FindExePath()
{
	TCHAR exeFullPath[MAX_PATH];
	if (GetModuleFileName(NULL, exeFullPath, MAX_PATH) == 0)
		rofirger::add_log(rofirger::LOG_LEVEL_ERROR, "get module{this.exe} file path fail!");
	CString szfolder = exeFullPath;
	int pos = szfolder.ReverseFind('\\');
	szfolder = szfolder.Left(pos);
	exe_path = szfolder;
}
