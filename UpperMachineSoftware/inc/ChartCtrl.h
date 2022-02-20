#pragma once


// ChartCtrl 对话框

class ChartCtrl : public CDialogEx
{
	DECLARE_DYNAMIC(ChartCtrl)

public:
	ChartCtrl(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~ChartCtrl();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PID };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
