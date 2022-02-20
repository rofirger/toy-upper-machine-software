
#include "pch.h"
#include "framework.h"
#include "UpperMachineSoftware.h"
#include "UpperMachineSoftwareDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CUARTDisplayApp
extern CString app_open_file_path;

BEGIN_MESSAGE_MAP(CUARTDisplayApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

// CUARTDisplayApp 构造

CUARTDisplayApp::CUARTDisplayApp()
{
	// 支持重新启动管理器
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}

// 唯一的 CUARTDisplayApp 对象

CUARTDisplayApp theApp;

// CUARTDisplayApp 初始化

BOOL CUARTDisplayApp::InitInstance()
{
	// 开启日志
	rofirger::Log::GetInstance()->StartLog();

	// 以该软件打开文件
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	app_open_file_path = cmdInfo.m_strFileName;
	if (!app_open_file_path.IsEmpty())
	{
		CString check_(".sfp");
		TCHAR tail_[5];
		tail_[4] = '\0';
		LPWSTR c_str_path = app_open_file_path.GetBuffer();
		int len = app_open_file_path.GetLength();
		tail_[0] = c_str_path[len - 4];
		tail_[1] = c_str_path[len - 3];
		tail_[2] = c_str_path[len - 2];
		tail_[3] = c_str_path[len - 1];

		rofirger::add_log(rofirger::LOG_LEVEL_INFO, "get file:%ls", c_str_path);

		if (CString(tail_) != check_)
		{
			app_open_file_path.Empty();
			rofirger::add_log(rofirger::LOG_LEVEL_WARNING, "can't open the file(file format error):%ls", c_str_path);
			::MessageBox(NULL, _T("仅能打开该软件创建的 .sfp 文件!"), _T("警告"), 0);
		}
		app_open_file_path.ReleaseBuffer();
	}
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。  否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	// 创建 shell 管理器，以防对话框包含
	// 任何 shell 树视图控件或 shell 列表视图控件。
	CShellManager* pShellManager = new CShellManager;

	// 激活“Windows Native”视觉管理器，以便在 MFC 控件中启用主题
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));

	CUpperMachineSoftwareDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: 在此放置处理何时用
		//  “确定”来关闭对话框的代码
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 在此放置处理何时用
		//  “取消”来关闭对话框的代码
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "警告: 对话框创建失败，应用程序将意外终止。\n");
		TRACE(traceAppMsg, 0, "警告: 如果您在对话框上使用 MFC 控件，则无法 #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS。\n");
	}

	// 删除上面创建的 shell 管理器。
	if (pShellManager != nullptr)
	{
		delete pShellManager;
	}

#if !defined(_AFXDLL) && !defined(_AFX_NO_MFC_CONTROLS_IN_DIALOGS)
	ControlBarCleanUp();
#endif

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。
	return FALSE;
}

CDocument* CUARTDisplayApp::OpenDocumentFile(LPCTSTR lpszFileName)
{
	// TODO: 在此添加专用代码和/或调用基类
	return CWinApp::OpenDocumentFile(lpszFileName);
}
