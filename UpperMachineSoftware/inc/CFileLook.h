#pragma once

// CFileLook 对话框
#include <vector>
typedef struct tagBITMAPINFO_X_
{
	BITMAPINFOHEADER  bmiHeader;
	RGBQUAD           bmiColors[256];
} BITMAPINFO_X_;
typedef struct PICINFO
{
	int pic_kind;
	int pic_height;
	int pic_width;
}PICINFO;
class CFileLook : public CDialogEx
{
	DECLARE_DYNAMIC(CFileLook)

public:
	//CFileLook(CWnd* pParent = nullptr);   // 标准构造函数
	CFileLook(std::vector<CString>& f, bool& close_param);
	void Reflesh();
	virtual ~CFileLook();

	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FILE_LOOK };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
private:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
private:
	std::vector<CString>& file;
	bool& close_;
	int index;
public:
	CListBox m_list;
	std::vector<unsigned char> now_pic;
	std::vector<PICINFO> pic_info;
	CSliderCtrl m_slider_zoom_raw;
	CToolTipCtrl m_toolTip;
	int m_zoom_val_raw;
	afx_msg void OnLbnSelchangeList1();
	afx_msg void OnPaint();
	afx_msg void OnNMCustomdrawSlider2(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedCheckPixelRaw();
	afx_msg void OnClose();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
