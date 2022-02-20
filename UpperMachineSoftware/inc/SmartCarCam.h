#pragma once

// SmartCarCam 对话框

class SmartCarCam : public CDialogEx
{
	DECLARE_DYNAMIC(SmartCarCam)

public:
	SmartCarCam(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~SmartCarCam();

	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SIMPLEINFO };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
