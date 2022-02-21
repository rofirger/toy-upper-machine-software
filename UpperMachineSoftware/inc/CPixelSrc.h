#pragma once

// CPixelSrc 对话框
#include "CImgProcess.h"
#include <vector>
class CPixelSrc : public CDialogEx
{
	DECLARE_DYNAMIC(CPixelSrc)

public:
	CPixelSrc(CWnd* pParent = nullptr);   // 标准构造函数
	CPixelSrc(CSmartCarCam* parent_class);
	void GetFileFromDirectory(CString csDirPath, std::vector<CString>& vctPath);
	void WriteSelectedFilePathToFile(std::string file_name, const std::vector<CString>& selected_file_name);
	virtual ~CPixelSrc();

	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PIXEL_SRC };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
private:
	CSmartCarCam* scc_class;
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCheck1();
	CDragListCtrl m_raw_pixel_src_name;
	CDragListCtrl m_raw_pixel_process_name;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton6();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedCancel();
	CEdit m_edit_interval;
	afx_msg void OnClose();
};
