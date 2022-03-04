// CPixelSrc.cpp: 实现文件
//

#include "pch.h"
#include "UpperMachineSoftware.h"
#include "CPixelSrc.h"
#include "afxdialogex.h"
#include <fstream>

// CPixelSrc 对话框

IMPLEMENT_DYNAMIC(CPixelSrc, CDialogEx)

CPixelSrc::CPixelSrc(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_PIXEL_SRC, pParent)
{
}
CPixelSrc::CPixelSrc(CImgProcess* parent_class)
	: CDialogEx(IDD_PIXEL_SRC)
{
	scc_class = parent_class;
}

CPixelSrc::~CPixelSrc()
{
}

void CPixelSrc::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_raw_pixel_src_name);
	DDX_Control(pDX, IDC_LIST3, m_raw_pixel_process_name);
	DDX_Control(pDX, IDC_EDIT_INTERVAL, m_edit_interval);
}

BEGIN_MESSAGE_MAP(CPixelSrc, CDialogEx)
	ON_BN_CLICKED(IDOK, &CPixelSrc::OnBnClickedOk)
	ON_BN_CLICKED(IDC_CHECK1, &CPixelSrc::OnBnClickedCheck1)
	ON_BN_CLICKED(IDC_BUTTON1, &CPixelSrc::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON6, &CPixelSrc::OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON5, &CPixelSrc::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON4, &CPixelSrc::OnBnClickedButton4)
	ON_BN_CLICKED(IDCANCEL, &CPixelSrc::OnBnClickedCancel)
	ON_WM_CLOSE()
END_MESSAGE_MAP()

// CPixelSrc 消息处理程序

void CPixelSrc::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	CString temp;
	GetDlgItemTextW(IDC_EDIT_INTERVAL, temp);
	scc_class->interval = _ttoi(temp);
	WriteSelectedFilePathToFile("pixel_file_selected.dat", scc_class->pixel_src_file_path_selected);
	CDialogEx::OnOK();
}
void CPixelSrc::GetFileFromDirectory(CString csDirPath, std::vector<CString>& vctPath)
{
	csDirPath += "*.sfp";
	HANDLE file;
	WIN32_FIND_DATA fileData;
	file = FindFirstFile(csDirPath.GetBuffer(), &fileData);
	if (file != INVALID_HANDLE_VALUE)
	{
		vctPath.push_back(fileData.cFileName);
		while (FindNextFile(file, &fileData))
		{
			vctPath.push_back(fileData.cFileName);
		}
	}
	csDirPath.ReleaseBuffer();
}
void CPixelSrc::WriteSelectedFilePathToFile(std::string file_name, const std::vector<CString>& selected_file_name)
{
	std::fstream file(file_name, std::ios::out);
	for (int i = 0; i < selected_file_name.size(); ++i)
	{
		std::string selected_file_name_temp_str((CW2A)selected_file_name[i].GetString());
		file << selected_file_name_temp_str.c_str();
		file << std::endl;
	}
	file.close();
}

void CPixelSrc::OnBnClickedCheck1()
{
	// TODO: 在此添加控件通知处理程序代码
	int state = ((CButton*)GetDlgItem(IDC_CHECK1))->GetCheck();
	if (state == BST_CHECKED)
	{
		scc_class->is_pixel_src_checked = true;
		CHAR            szFolderPath[255] = { 0 };
		CString          strFolderPath;
		BROWSEINFO       sInfo;
		::ZeroMemory(&sInfo, sizeof(BROWSEINFO));
		sInfo.pidlRoot = 0;
		sInfo.lpszTitle = _T("请选择像素源文件夹");
		sInfo.ulFlags = BIF_RETURNONLYFSDIRS | BIF_EDITBOX | BIF_DONTGOBELOWDOMAIN;
		sInfo.lpfn = NULL;

		LPITEMIDLIST    lpidlBrowse = ::SHBrowseForFolder(&sInfo);
		if (lpidlBrowse != NULL)
		{
			if (::SHGetPathFromIDListA(lpidlBrowse, szFolderPath))
			{
				strFolderPath = szFolderPath;
			}
		}
		if (lpidlBrowse != NULL)
		{
			::CoTaskMemFree(lpidlBrowse);
		}
		strFolderPath += CString("\\");
		scc_class->pixel_src_path = strFolderPath;
		this->SetDlgItemTextW(IDC_PIXEL_SRC, strFolderPath);
		scc_class->pixel_src_file_path.clear();
		GetFileFromDirectory(strFolderPath, scc_class->pixel_src_file_path);
		scc_class->pixel_src_path = strFolderPath;
		// 更新listCtrl
		for (int i = m_raw_pixel_src_name.GetItemCount() - 1; i >= 0; --i)
		{
			m_raw_pixel_src_name.DeleteItem(i);
		}
		//m_raw_pixel_src_name.InsertColumn(0, _T("0"), LVCFMT_CENTER, 132);
		//m_raw_pixel_src_name.InsertColumn(1, _T("选定文件夹下的(*.sfp)文件"), LVCFMT_CENTER, 372);
		//m_raw_pixel_src_name.DeleteColumn(0); // 删除第0列
		for (int i = 0; i < scc_class->pixel_src_file_path.size(); ++i)
		{
			int row = m_raw_pixel_src_name.InsertItem(i, _T("0"));
			m_raw_pixel_src_name.SetItemText(row, 0, scc_class->pixel_src_file_path[i]);
		}
		CString str_static_num("文件数:");
		str_static_num += std::to_string(m_raw_pixel_src_name.GetItemCount()).c_str();
		SetDlgItemTextW(IDC_STATIC_RAW_NUM, str_static_num);
		// 用于给像素图加入图片信息
		//int wid = 152;
		//int hei = 50;
		//int ki = 0;
		//for (int i = 0; i < scc_class->pixel_src_file_path.size(); ++i)
		//{
		//	CString temp = scc_class->pixel_src_path + scc_class->pixel_src_file_path[i];
		//	std::fstream out_file;
		//	out_file.open(temp.GetBuffer(), std::ios::in | std::ios::out | std::ios::binary);
		//	out_file.seekp(0, std::ios::end);
		//	out_file.write((char*)&wid, sizeof(wid));
		//	out_file.write((char*)&hei, sizeof(hei));
		//	out_file.write((char*)&ki, sizeof(ki));
		//	out_file.close();

		//	/*int wid_;
		//	int hei_;
		//	int ki_;
		//	std::fstream out_file1;
		//	out_file1.open(temp.GetBuffer(), std::ios::in | std::ios::out | std::ios::binary);
		//	out_file1.seekg(-(int)sizeof(int), std::ios::end);
		//	out_file1.read((char*)&wid_, sizeof(wid_));
		//	out_file1.seekg(-(int)sizeof(int) * 2, std::ios::end);
		//	out_file1.read((char*)&hei_, sizeof(hei_));
		//	out_file1.seekg(-(int)sizeof(int) * 3, std::ios::end);
		//	out_file1.read((char*)&ki_, sizeof(ki_));
		//	out_file1.close();*/
		//	temp.ReleaseBuffer();
		//}
	}
	else
	{
		scc_class->is_pixel_src_checked = false;
		scc_class->pixel_src_file_path_selected.clear();
	}
}

BOOL CPixelSrc::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	if (scc_class->is_pixel_src_checked)
	{
		((CButton*)GetDlgItem(IDC_CHECK1))->SetCheck(1);
		this->SetDlgItemTextW(IDC_PIXEL_SRC, scc_class->pixel_src_path);
	}
	for (int i = m_raw_pixel_src_name.GetItemCount() - 1; i >= 0; --i)
	{
		m_raw_pixel_src_name.DeleteItem(i);
	}
	//m_raw_pixel_src_name.InsertColumn(0, _T("0"), LVCFMT_CENTER, 132);
	m_raw_pixel_src_name.InsertColumn(0, _T("选定文件夹下的(*.sfp)文件"), LVCFMT_CENTER, 372);
	//m_raw_pixel_src_name.DeleteColumn(0); // 删除第0列
	for (int i = 0; i < scc_class->pixel_src_file_path.size(); ++i)
	{
		int row = m_raw_pixel_src_name.InsertItem(i, _T("0"));
		m_raw_pixel_src_name.SetItemText(row, 0, scc_class->pixel_src_file_path[i]);
	}
	CString str_static_num("文件数:");
	str_static_num += std::to_string(scc_class->pixel_src_file_path.size()).c_str();
	SetDlgItemTextW(IDC_STATIC_RAW_NUM, str_static_num);

	for (int i = m_raw_pixel_process_name.GetItemCount() - 1; i >= 0; --i)
	{
		m_raw_pixel_process_name.DeleteItem(i);
	}
	//m_raw_pixel_process_name.InsertColumn(0, _T("0"), LVCFMT_CENTER, 132);
	m_raw_pixel_process_name.InsertColumn(0, _T("选定的(*.sfp)文件"), LVCFMT_CENTER, 372);
	//m_raw_pixel_process_name.DeleteColumn(0); // 删除第0列
	for (int i = 0; i < scc_class->pixel_src_file_path_selected.size(); ++i)
	{
		int row = m_raw_pixel_process_name.InsertItem(i, _T("0"));
		m_raw_pixel_process_name.SetItemText(row, 0, scc_class->pixel_src_file_path_selected[i]);
	}
	CString str_static_num_process("文件数:");
	str_static_num_process += std::to_string(scc_class->pixel_src_file_path_selected.size()).c_str();
	SetDlgItemTextW(IDC_STATIC_PROCESS_NUM, str_static_num_process);
	SetDlgItemTextW(IDC_EDIT_INTERVAL, CString(std::to_string(scc_class->interval).c_str()));

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

void CPixelSrc::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	POSITION pos = m_raw_pixel_src_name.GetFirstSelectedItemPosition();
	if (pos != NULL)
	{
		while (pos)
		{
			int nItem = m_raw_pixel_src_name.GetNextSelectedItem(pos);
			scc_class->pixel_src_file_path_selected.push_back(m_raw_pixel_src_name.GetItemText(nItem, 0));
			// nItem是所选中行的序号
		}
	}
	// 更新
	for (int i = m_raw_pixel_process_name.GetItemCount() - 1; i >= 0; --i)
	{
		m_raw_pixel_process_name.DeleteItem(i);
	}
	for (int i = 0; i < scc_class->pixel_src_file_path_selected.size(); ++i)
	{
		int row = m_raw_pixel_process_name.InsertItem(i, _T("0"));
		m_raw_pixel_process_name.SetItemText(row, 0, scc_class->pixel_src_file_path_selected[i]);
	}
	CString str_static_num_process("文件数:");
	str_static_num_process += std::to_string(scc_class->pixel_src_file_path_selected.size()).c_str();
	SetDlgItemTextW(IDC_STATIC_PROCESS_NUM, str_static_num_process);
}

void CPixelSrc::OnBnClickedButton6()
{
	// TODO: 在此添加控件通知处理程序代码
	scc_class->pixel_src_file_path_selected.clear();
	for (int i = 0; i < scc_class->pixel_src_file_path.size(); ++i)
	{
		scc_class->pixel_src_file_path_selected.push_back(scc_class->pixel_src_file_path[i]);
	}
	for (int i = m_raw_pixel_process_name.GetItemCount() - 1; i >= 0; --i)
	{
		m_raw_pixel_process_name.DeleteItem(i);
	}
	for (int i = 0; i < scc_class->pixel_src_file_path_selected.size(); ++i)
	{
		int row = m_raw_pixel_process_name.InsertItem(i, _T("0"));
		m_raw_pixel_process_name.SetItemText(row, 0, scc_class->pixel_src_file_path_selected[i]);
	}
	CString str_static_num_process("文件数:");
	str_static_num_process += std::to_string(scc_class->pixel_src_file_path_selected.size()).c_str();
	SetDlgItemTextW(IDC_STATIC_PROCESS_NUM, str_static_num_process);
}

void CPixelSrc::OnBnClickedButton5()
{
	// TODO: 在此添加控件通知处理程序代码
	scc_class->pixel_src_file_path_selected.clear();

	for (int i = m_raw_pixel_process_name.GetItemCount() - 1; i >= 0; --i)
	{
		m_raw_pixel_process_name.DeleteItem(i);
	}
	CString str_static_num_process("文件数:");
	str_static_num_process += std::to_string(scc_class->pixel_src_file_path_selected.size()).c_str();
	SetDlgItemTextW(IDC_STATIC_PROCESS_NUM, str_static_num_process);
}

void CPixelSrc::OnBnClickedButton4()
{
	// TODO: 在此添加控件通知处理程序代码
	POSITION pos = m_raw_pixel_process_name.GetFirstSelectedItemPosition();
	int m = 0;
	if (pos != NULL)
	{
		while (pos)
		{
			int nItem = m_raw_pixel_process_name.GetNextSelectedItem(pos);
			auto iter_ = scc_class->pixel_src_file_path_selected.begin() + nItem - m++;
			scc_class->pixel_src_file_path_selected.erase(iter_);
		}
	}
	for (int i = m_raw_pixel_process_name.GetItemCount() - 1; i >= 0; --i)
	{
		m_raw_pixel_process_name.DeleteItem(i);
	}
	for (int i = 0; i < scc_class->pixel_src_file_path_selected.size(); ++i)
	{
		int row = m_raw_pixel_process_name.InsertItem(i, _T("0"));
		m_raw_pixel_process_name.SetItemText(row, 0, scc_class->pixel_src_file_path_selected[i]);
	}
	CString str_static_num_process("文件数:");
	str_static_num_process += std::to_string(scc_class->pixel_src_file_path_selected.size()).c_str();
	SetDlgItemTextW(IDC_STATIC_PROCESS_NUM, str_static_num_process);
}

void CPixelSrc::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnCancel();
}


void CPixelSrc::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	WriteSelectedFilePathToFile("pixel_file_selected.dat", scc_class->pixel_src_file_path_selected);
	CDialogEx::OnClose();
}
