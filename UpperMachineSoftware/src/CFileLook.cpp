#include "pch.h"
#include "UpperMachineSoftware.h"
#include "CFileLook.h"
#include "afxdialogex.h"
#include <fstream>
#include <opencv2/opencv.hpp>
#include <opencv2/core/utils/logger.hpp>
IMPLEMENT_DYNAMIC(CFileLook, CDialogEx)
CFileLook::CFileLook(std::vector<CString>& f, bool& close_param) : file(f), close_(close_param) {}

CFileLook::~CFileLook()
{
}

void CFileLook::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_list);
	DDX_Control(pDX, IDC_SLIDER2, m_slider_zoom_raw);
	HICON hIcon = AfxGetApp()->LoadIconW(IDR_MAINFRAME);
	SetIcon(hIcon, FALSE);
}

BEGIN_MESSAGE_MAP(CFileLook, CDialogEx)
	ON_LBN_SELCHANGE(IDC_LIST1, &CFileLook::OnLbnSelchangeList1)
	ON_WM_PAINT()
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER2, &CFileLook::OnNMCustomdrawSlider2)
	ON_BN_CLICKED(IDC_CHECK_PIXEL_RAW, &CFileLook::OnBnClickedCheckPixelRaw)
	ON_WM_CLOSE()
	ON_WM_DROPFILES()
	ON_WM_SIZE()
END_MESSAGE_MAP()

void Split(CString source, CStringArray& dest, CString division)
{
	dest.RemoveAll();
	int pos = 0;
	int pre_pos = 0;
	while (-1 != pos)
	{
		pre_pos = pos;
		pos = source.Find(division, (pos + 1));
		dest.Add(source.Mid(pre_pos + 1, source.GetLength() - pre_pos));
	}
}
BOOL CFileLook::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	close_ = false;

	m_slider_zoom_raw.SetRange(1, 10);
	m_slider_zoom_raw.SetTicFreq(1);
	m_slider_zoom_raw.SetPos(1);
	for (int i = 0; i < file.size(); ++i)
	{
		CStringArray dest;
		Split(file[i], dest, CString("\\"));
		CString temp = dest[dest.GetSize() - 1];
		m_list.AddString(temp);

		PICINFO temp_info;
		pic_info.push_back(temp_info);
		std::fstream out_file1;
		out_file1.open(file[i].GetBuffer(), std::ios::in | std::ios::out | std::ios::binary);
		out_file1.seekg(-(int)sizeof(int), std::ios::end);
		out_file1.read((char*)&pic_info[i].pic_kind, sizeof(pic_info[i].pic_kind));
		out_file1.seekg(-(int)sizeof(int) * 2, std::ios::end);
		out_file1.read((char*)&pic_info[i].pic_height, sizeof(pic_info[i].pic_height));
		out_file1.seekg(-(int)sizeof(int) * 3, std::ios::end);
		out_file1.read((char*)&pic_info[i].pic_width, sizeof(pic_info[i].pic_width));
		out_file1.close();
		file[i].ReleaseBuffer();
	}
	m_list.SetCurSel(0);

	return TRUE;
}

void CFileLook::OnLbnSelchangeList1()
{
	CRect rect;
	GetDlgItem(IDC_STATIC_PIC)->GetClientRect(&rect);  // 获取图片控件矩形框
	InvalidateRect(&rect);
}
void CFileLook::Reflesh()
{
	for (int i = m_list.GetCount() - 1; i >= 0; --i)
	{
		m_list.DeleteString(i);
	}
	for (int i = 0; i < file.size(); ++i)
	{
		CStringArray dest;
		Split(file[i], dest, CString("\\"));
		CString temp = dest[dest.GetSize() - 1];
		m_list.AddString(temp);
		PICINFO temp_info;
		pic_info.push_back(temp_info);
		std::fstream out_file1;
		out_file1.open(file[i].GetBuffer(), std::ios::in | std::ios::out | std::ios::binary);
		out_file1.seekg(-(int)sizeof(int), std::ios::end);
		out_file1.read((char*)&pic_info[i].pic_kind, sizeof(pic_info[i].pic_kind));
		out_file1.seekg(-(int)sizeof(int) * 2, std::ios::end);
		out_file1.read((char*)&pic_info[i].pic_height, sizeof(pic_info[i].pic_height));
		out_file1.seekg(-(int)sizeof(int) * 3, std::ios::end);
		out_file1.read((char*)&pic_info[i].pic_width, sizeof(pic_info[i].pic_width));
		out_file1.close();
		file[i].ReleaseBuffer();
	}
	m_list.SetCurSel(index);
}
void CFileLook::OnPaint()
{
	CPaintDC dc(this);

	index = m_list.GetCurSel();
	if (index >= 0)
	{
		int pic_size;
		pic_size = pic_info[index].pic_height * pic_info[index].pic_width;
		std::fstream read_file;
		read_file.open(file[index].GetBuffer(), std::ios::in | std::ios::binary);
		SetDlgItemText(IDC_PATH, file[index]);

		unsigned char val_;
		now_pic.clear();
		int i = 0;
		while (read_file.read((char*)&val_, sizeof(val_)))
		{
			now_pic.push_back(val_);
			i++;
			if (i >= pic_size)
			{
				break;
			}
		}
		read_file.close();
		file[index].ReleaseBuffer();
		cv::Mat img(pic_info[index].pic_height, (size_t)pic_info[index].pic_width, CV_8UC1);
		int index_ = 0;
		//std::ofstream tt("ttt.txt");
		for (size_t i = 0; i < pic_info[index].pic_height; i++)
		{
			for (size_t j = 0; j < pic_info[index].pic_width; j++)
			{
				if (index_ < now_pic.size())
				{
					/*int temo = now_pic[index_];
					tt << (char)temo;
					tt << ", ";*/
					img.at<uchar>(i, j) = now_pic[index_++];
				}
			}
		}
		//tt.close();
		CRect rect;
		GetDlgItem(IDC_STATIC_PIC)->GetClientRect(&rect);  // 获取图片控件矩形框
		int is_zoom_pixel_raw = ((CButton*)GetDlgItem(IDC_CHECK_PIXEL_RAW))->GetCheck();
		int max_zoom_val = rect.Width() / img.cols < rect.Height() / img.rows ? rect.Width() / img.cols : rect.Height() / img.rows;
		int zoom_val_raw = m_slider_zoom_raw.GetPos() < max_zoom_val ? m_slider_zoom_raw.GetPos() : max_zoom_val;
		m_zoom_val_raw = zoom_val_raw;
		m_slider_zoom_raw.SetPos(zoom_val_raw);

		// 缩放Mat
		if (is_zoom_pixel_raw == BST_CHECKED)
		{
			cv::Mat temp = img;
			cv::resize(img, img, cv::Size(img.cols * zoom_val_raw, img.rows * zoom_val_raw));
			for (int i = 0; i < img.rows; ++i)
			{
				for (int j = 0; j < img.cols; ++j)
				{
					img.at<uchar>(i, j) = temp.at<uchar>(i / zoom_val_raw, j / zoom_val_raw);
				}
			}
		}
		else
		{
			cv::resize(img, img, cv::Size(img.cols * zoom_val_raw, img.rows * zoom_val_raw));
		}

		int pixelBytes = img.channels() * (img.depth() + 1); // 计算一个像素多少个字节
		// 制作bitmapinfo(数据头)
		BITMAPINFO bitInfo;
		bitInfo.bmiHeader.biBitCount = 8 * pixelBytes;
		bitInfo.bmiHeader.biWidth = img.cols;
		bitInfo.bmiHeader.biHeight = -img.rows;   //注意"-"号(正数时倒着绘制)
		bitInfo.bmiHeader.biPlanes = 1;
		bitInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bitInfo.bmiHeader.biCompression = BI_RGB;
		bitInfo.bmiHeader.biClrImportant = 0;
		bitInfo.bmiHeader.biClrUsed = 0;
		bitInfo.bmiHeader.biSizeImage = 0;
		bitInfo.bmiHeader.biXPelsPerMeter = 0;
		bitInfo.bmiHeader.biYPelsPerMeter = 0;
		BITMAPINFO_X_ pBmpInfo;
		memcpy(&pBmpInfo.bmiHeader, &bitInfo, sizeof(BITMAPINFOHEADER));
		//构造灰度图的调色版
		for (int i = 0; i < 256; i++)
		{
			pBmpInfo.bmiColors[i].rgbBlue = i;
			pBmpInfo.bmiColors[i].rgbGreen = i;
			pBmpInfo.bmiColors[i].rgbRed = i;
			pBmpInfo.bmiColors[i].rgbReserved = 0;
		}
		int nWindowW = rect.Width();				//获取窗口宽度
		int nWindowH = rect.Height();				//获取窗口高度
		int nImageW = img.cols;				//获取图片宽度
		int nImageH = img.rows;				//获取图片高度

		CDC* pDC = GetDlgItem(IDC_STATIC_PIC)->GetDC();  //获取图片控件DC
			//绘图
		::StretchDIBits(
			pDC->GetSafeHdc(),
			(int)(nWindowW - nImageW) / 2, (int)(nWindowH - nImageH) / 2, img.cols, img.rows,
			0, 0, img.cols, img.rows,
			img.data,
			(BITMAPINFO*)&pBmpInfo,
			DIB_RGB_COLORS,
			SRCCOPY
		);
		ReleaseDC(pDC);  //释放DC
	}

	// 不为绘图消息调用 CDialogEx::OnPaint()
}

void CFileLook::OnNMCustomdrawSlider2(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	CRect rect;
	GetDlgItem(IDC_STATIC_PIC)->GetClientRect(&rect);  // 获取图片控件矩形框
	InvalidateRect(&rect);
	/*CString str;
	str.Format(_T("%d"), m_slider_zoom_raw.GetPos());
	m_toolTip.UpdateTipText(str, GetDlgItem(IDC_SLIDER2));*/
	*pResult = 0;
}

void CFileLook::OnBnClickedCheckPixelRaw()
{
	// TODO: 在此添加控件通知处理程序代码
	CRect rect;
	GetDlgItem(IDC_STATIC_PIC)->GetClientRect(&rect);  // 获取图片控件矩形框
	InvalidateRect(&rect);
}

void CFileLook::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	close_ = true;
	CDialogEx::OnClose();
}

BOOL CFileLook::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (pMsg->message == WM_KEYDOWN)
	{
		switch (pMsg->wParam)
		{
		case VK_DELETE:
		{
			int n_item = m_list.GetCurSel();
			if (n_item >= 0)
			{
				m_list.DeleteString(n_item);
				auto iter_ = file.begin();
				file.erase(iter_ + n_item);
			}
			if (n_item < m_list.GetCount())
			{
				Reflesh();
			}

			break;
		}
		case VK_RETURN: // 避免dlg窗口编辑框按回车(Enter)键和ESC键会退出
			return TRUE;
		case VK_ESCAPE: // // 避免dlg窗口编辑框按回车(Enter)键和ESC键会退出
			return TRUE;
		default:
			break;
		}
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}

void CFileLook::OnDropFiles(HDROP hDropInfo)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	int  nFileCount = ::DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 256);   //获取拖入的文件数量

	for (int i = 0; i < nFileCount; i++)
	{
		CString m_file;// 完整路径
		UINT nChars = ::DragQueryFile(hDropInfo, i, m_file.GetBuffer(256), 256);   // 获取拖入的第i个文件的文件名
		m_file.ReleaseBuffer();
		file.push_back(m_file);
	}
	Reflesh();
	CDialogEx::OnDropFiles(hDropInfo);
}

void CFileLook::OnSize(UINT nType, int cx, int cy)
{
	Invalidate(TRUE);
	CDialogEx::OnSize(nType, cx, cy);
	CRect client_rect;
	GetClientRect(&client_rect);

	CRect rect_list_file;
	CRect rect_static_pic;
	CRect rect_edit_path;
	CRect rect_slider_zoom;
	CRect rect_check_pixel;
	CWnd* pWnd_list_file = GetDlgItem(IDC_LIST1);
	CWnd* pWnd_static_pic = GetDlgItem(IDC_STATIC_PIC);
	CWnd* pWnd_edit_path = GetDlgItem(IDC_PATH);
	CWnd* pWnd_slider_zoom = GetDlgItem(IDC_SLIDER2);
	CWnd* pWnd_check_pixel = GetDlgItem(IDC_CHECK_PIXEL_RAW);
	if (pWnd_check_pixel)
	{
		pWnd_list_file->GetWindowRect(&rect_list_file);
		ScreenToClient(&rect_list_file);
		pWnd_static_pic->GetWindowRect(&rect_static_pic);
		ScreenToClient(&rect_static_pic);
		pWnd_edit_path->GetWindowRect(&rect_edit_path);
		ScreenToClient(&rect_edit_path);
		pWnd_slider_zoom->GetWindowRect(&rect_slider_zoom);
		ScreenToClient(&rect_slider_zoom);
		pWnd_check_pixel->GetWindowRect(&rect_check_pixel);
		ScreenToClient(&rect_check_pixel);

		pWnd_list_file->SetWindowPos(NULL, client_rect.left + 10, client_rect.top + 10, rect_list_file.Width(), client_rect.Height() - 20, SWP_NOZORDER);
		pWnd_list_file->GetWindowRect(&rect_list_file);
		ScreenToClient(&rect_list_file);
		pWnd_static_pic->SetWindowPos(NULL, rect_list_file.right, rect_list_file.top, client_rect.right - rect_list_file.right - 10, rect_list_file.Height() - rect_edit_path.Height() - rect_slider_zoom.Height() - 25, SWP_NOZORDER);
		pWnd_static_pic->GetWindowRect(&rect_static_pic);
		ScreenToClient(&rect_static_pic);
		pWnd_edit_path->SetWindowPos(NULL, rect_static_pic.left, rect_static_pic.bottom, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		pWnd_edit_path->GetWindowRect(&rect_edit_path);
		ScreenToClient(&rect_edit_path);
		pWnd_slider_zoom->SetWindowPos(NULL, rect_list_file.right + (rect_static_pic.Width() - rect_slider_zoom.Width() - rect_check_pixel.Width() - 10) / 2, rect_edit_path.bottom + 15, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		pWnd_slider_zoom->GetWindowRect(&rect_slider_zoom);
		ScreenToClient(&rect_slider_zoom);
		pWnd_check_pixel->SetWindowPos(NULL, rect_slider_zoom.right + 10, rect_slider_zoom.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		pWnd_check_pixel->GetWindowRect(&rect_check_pixel);
		ScreenToClient(&rect_check_pixel);
		if (pic_info[index].pic_width != 0)
		{
			int height_times = rect_static_pic.Height() / pic_info[index].pic_height;
			int width_times = rect_static_pic.Width() / pic_info[index].pic_width;
			int min_val = height_times < width_times ? height_times : width_times;
			m_slider_zoom_raw.SetRange(1, min_val);
		}
	}
}
