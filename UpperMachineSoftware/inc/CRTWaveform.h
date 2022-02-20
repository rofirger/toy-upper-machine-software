#pragma once

#include "ChartClass\ChartCtrl.h"
#include "ChartClass\ChartLineSerie.h"
#include "ChartClass\ChartAxis.h"
#include "uart.h"
#include "timer.h"
#include <list>
#include <vector>
// 可绘制含颜色的listbox
class CColorListBox :public CCheckListBox
{
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	int AddString(LPCTSTR lpszItem, COLORREF itemColor = RGB(255, 0, 0));
private:
};
// CRTwaveform 对话框
class CRTwaveform : public CDialogEx
{
	DECLARE_DYNAMIC(CRTwaveform)

public:
	CRTwaveform(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CRTwaveform();

	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PID };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CMFCTabCtrl m_tab;
	CChartCtrl m_HSChartCtrl;
	//CChartLineSerie* m_pLineSerie;
	std::vector<CChartLineSerie*>m_pLineSeries;
	virtual BOOL OnInitDialog();
public:
	CSerialPort mySerialPort;
	Timer reconnect_timer;
private:
	CComboBox m_combo_coms;
	CComboBox m_combo_baudrate;
	CComboBox m_combo_databit;
	CComboBox m_combo_check;
	CComboBox m_combo_stopbit;
	CButton m_check_noerr;
	CButton m_check_reconnect;
public:
	afx_msg void OnBnClickedButton3();
	void EnableControlPid(bool true_or_false);
	void ReadCom();
	volatile bool IS_STOP_RECONNECT = false;
	CColorListBox m_list_check;
	afx_msg void OnLbnSelchangeList();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//private:
	//	CFont m_font;
public:
	// CColorListBox m_listbox_color_box;
	afx_msg void OnSize(UINT nType, int cx, int cy);
private:
	// 改变窗体大小
	CRect rect_s_1;
	CRect rect_s_2;
	CRect rect_s_3;
	CRect rect_s_4;
	CRect rect_s_5;
	CRect rect_coms;
	CRect rect_baudrate;
	CRect rect_databits;
	CRect rect_check;
	CRect rect_stopbit;
	CRect rect_noerr;
	CRect rect_reconnect;
	CRect rect_button;
};

