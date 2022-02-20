
// UpdateDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "Update.h"
#include "UpdateDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CUpdateDlg 对话框

CUpdateDlg::CUpdateDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_UPDATE_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}
CUpdateDlg::~CUpdateDlg()
{
	KillTimer(1);
}
void CUpdateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS1, m_progress);
}

BEGIN_MESSAGE_MAP(CUpdateDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
END_MESSAGE_MAP()

// CUpdateDlg 消息处理程序
CString exe_path;
void FindExePath()
{
	TCHAR exeFullPath[MAX_PATH];
	GetModuleFileName(NULL, exeFullPath, MAX_PATH);
	CString szfolder = exeFullPath;
	int pos = szfolder.ReverseFind('\\');
	szfolder = szfolder.Left(pos);
	exe_path = szfolder;
}
void DownloadFile(CString szUrl, CString szPath)
{
	CBindCallback cbc;
	/*cbc.m_pdlg = this;*/

	//在url后添加随机数，防止从IE缓存中读取。url后加随机数不会影响下载的。
	//如果想要从缓存中提取那么就把下面的注释掉
	CString szRand;
	szRand.Format(_T("?skq=%d"), GetTickCount());
	szUrl += szRand;

	URLDownloadToFile(NULL, szUrl, szPath, 0, &cbc);//szURL,szPath为全局变量
		//::MessageBox(NULL, _T("FINISHED"), _T("提示"), 0);
}
int now_pos = 0;
int total_pos = 0;
void DownLoad()
{
	FindExePath();
	CString csIniFile = exe_path + _T("\\ver\\smcd_update.ini");
	CString szValue;
	::GetPrivateProfileString(_T("FILENUM"), _T("num"), NULL, szValue.GetBuffer(MAX_PATH), MAX_PATH, csIniFile);;
	szValue.ReleaseBuffer();

	std::string file_num_str(CW2A(szValue.GetString()));
	int file_num = atoi(file_num_str.c_str());
	CString file_path;
	CString file_url;
	total_pos = file_num;
	for (int i = 0; i < file_num; ++i)
	{
		now_pos = i;
		CString temp;
		temp.Format(_T("%d"), i);
		::GetPrivateProfileString(_T("FILEPATH"), _T("path") + temp, _T("Default"), file_path.GetBuffer(MAX_PATH), MAX_PATH, csIniFile);
		file_path.ReleaseBuffer();
		::GetPrivateProfileString(_T("URL"), _T("url") + temp, _T("Default"), file_url.GetBuffer(MAX_PATH), MAX_PATH, csIniFile);
		file_url.ReleaseBuffer();
		DownloadFile(file_url, exe_path + _T("\\") + file_path);
	}
	CString strFileName_ini;
	strFileName_ini = exe_path + _T("\\ver\\smcd_update.ini");
	::DeleteFile(strFileName_ini);						//删除下载来的配置文件
	MessageBox(NULL, _T("更新完毕"), _T("提示"), 0);
	ExitProcess(0);
}
BOOL CUpdateDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	_temer.AsyncWait(100, DownLoad);
	SetTimer(1, 1000, NULL);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CUpdateDlg::OnPaint()
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
HCURSOR CUpdateDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
// 只需实现OnProgress方法，类的实现：
CBindCallback::CBindCallback()
{
}

CBindCallback::~CBindCallback()
{
}

//仅实现OnProgress成员即可

HRESULT CBindCallback::OnProgress(ULONG ulProgress,
	ULONG ulProgressMax,
	ULONG ulStatusCode,
	LPCWSTR wszStatusText)
{
	return S_OK;
}

void CUpdateDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	switch (nIDEvent)
	{
	case 1:
	{
		CProgressCtrl* m_prg = (CProgressCtrl*)this->GetDlgItem(IDC_PROGRESS1);
		m_prg->SetRange32(0, total_pos - 1);
		m_prg->SetPos(now_pos);
		break;
	}
	default:
		break;
	}
	CDialogEx::OnTimer(nIDEvent);
}
