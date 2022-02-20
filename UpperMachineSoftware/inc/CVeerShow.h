#pragma once

// CVeerShow 对话框
class CVeerShow : public CDialogEx
{
	DECLARE_DYNAMIC(CVeerShow)

public:
	CVeerShow(CWnd* pParent, CVeerShow** pThis, double* slope_param);   // 标准构造函数
	virtual ~CVeerShow();

	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_VEER };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
private:
	CVeerShow** _this;
	double* slope;
	double last_slope_angle;
	double degree;
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
