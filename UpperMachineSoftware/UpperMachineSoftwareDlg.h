#pragma once
#include "CSimpleInfo.h"
#include "CImgProcess.h"
#include "CRTWaveform.h"
#include <vector>

// CUpperMachineSoftwareDlg 对话框
class CUpperMachineSoftwareDlg : public CDialogEx
{
	// 构造
public:
	CUpperMachineSoftwareDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_UPPERMACHINE_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
private:
	SimpleInfo uart_simple;
	CSmartCarCam uart_pic;
	CRTwaveform uart_rtwaveform;
	std::vector<CString> main_dlg_drop_files;
public:
	CTabCtrl m_tab;
	afx_msg void OnTcnSelchangeTab1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClose();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	void DownloadFile(CString szUrl, CString szPath);
public:
	CString exe_path;
	Timer _timer_for_check_ver;
	void FindExePath();
	void CheckVer();
};
class CBindCallback : public IBindStatusCallback
{
private:
	//CUpdate* m_pdlg;
public:
	CBindCallback();
	virtual ~CBindCallback();

	//IBindStatusCallback的方法。除了OnProgress     外的其他方法都返回E_NOTIMPL

	STDMETHOD(OnStartBinding)
		(DWORD dwReserved,
			IBinding __RPC_FAR* pib)
	{
		return E_NOTIMPL;
	}

	STDMETHOD(GetPriority)
		(LONG __RPC_FAR* pnPriority)
	{
		return E_NOTIMPL;
	}

	STDMETHOD(OnLowResource)
		(DWORD reserved)
	{
		return E_NOTIMPL;
	}

	//OnProgress在这里
	STDMETHOD(OnProgress)
		(ULONG ulProgress,
			ULONG ulProgressMax,
			ULONG ulStatusCode,
			LPCWSTR wszStatusText);

	STDMETHOD(OnStopBinding)
		(HRESULT hresult,
			LPCWSTR szError)
	{
		return E_NOTIMPL;
	}

	STDMETHOD(GetBindInfo)
		(DWORD __RPC_FAR* grfBINDF,
			BINDINFO __RPC_FAR* pbindinfo)
	{
		return E_NOTIMPL;
	}

	STDMETHOD(OnDataAvailable)
		(DWORD grfBSCF,
			DWORD dwSize,
			FORMATETC __RPC_FAR* pformatetc,
			STGMEDIUM __RPC_FAR* pstgmed)
	{
		return E_NOTIMPL;
	}

	STDMETHOD(OnObjectAvailable)
		(REFIID riid,
			IUnknown __RPC_FAR* punk)
	{
		return E_NOTIMPL;
	}

	// IUnknown方法.IE 不会调用这些方法的

	STDMETHOD_(ULONG, AddRef)()
	{
		return 0;
	}

	STDMETHOD_(ULONG, Release)()
	{
		return 0;
	}

	STDMETHOD(QueryInterface)
		(REFIID riid,
			void __RPC_FAR* __RPC_FAR* ppvObject)
	{
		return E_NOTIMPL;
	}
};