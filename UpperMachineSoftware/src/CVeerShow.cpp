// CVeerShow.cpp: 实现文件
//

#include "pch.h"
#include "UpperMachineSoftware.h"
#include "CVeerShow.h"
#include "afxdialogex.h"

// CVeerShow 对话框
const UINT ID_FLASH_SLOPE = 0x1001;
IMPLEMENT_DYNAMIC(CVeerShow, CDialogEx)

CVeerShow::CVeerShow(CWnd* pParent /*=nullptr*/, CVeerShow** pThis, double* slope_param)
	: CDialogEx(IDD_VEER, pParent)
{
	//this->SetOwner(pParent);
	_this = pThis;
	slope = slope_param;
}

CVeerShow::~CVeerShow()
{
	KillTimer(ID_FLASH_SLOPE);
}

void CVeerShow::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CVeerShow, CDialogEx)
	ON_WM_CLOSE()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_SIZE()
END_MESSAGE_MAP()

// CVeerShow 消息处理程序

BOOL CVeerShow::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	degree = 0.8;
	::SetWindowLong(GetSafeHwnd(), GWL_EXSTYLE, ::GetWindowLongPtr(GetSafeHwnd(), GWL_EXSTYLE) | WS_EX_LAYERED);
	this->SetLayeredWindowAttributes(0, 255 * 80 * degree, LWA_ALPHA);
	CRect rect_this;
	GetClientRect(&rect_this);
	SetTimer(ID_FLASH_SLOPE, 100, NULL);

	return TRUE;
}

void CVeerShow::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	*_this = NULL;
	CDialogEx::OnClose();
}

void CVeerShow::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CRect rect_this;
	GetClientRect(&rect_this);
	if (point.y > rect_this.bottom - 10)
	{
		HCURSOR hCur = LoadCursor(NULL, IDC_HAND);
		::SetCursor(hCur);
	}

	CDialogEx::OnMouseMove(nFlags, point);
}

void CVeerShow::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CRect rect_this;
	GetClientRect(&rect_this);
	if (point.y > rect_this.bottom - 10)
	{
		CRect rect_this;
		GetClientRect(&rect_this);
		degree = (double)point.x / rect_this.Width();
		CClientDC dc(this);
		CPen pen_old(PS_SOLID, 5, RGB(220, 220, 220));
		CPen* p_old_pen = dc.SelectObject(&pen_old);
		dc.MoveTo(rect_this.left, rect_this.bottom);
		dc.LineTo(rect_this.right, rect_this.bottom);
		DeleteObject(&pen_old);

		CPen pen(PS_SOLID, 5, RGB(0, 153, 51));
		CPen* pPen = dc.SelectObject(&pen);
		dc.MoveTo(rect_this.left, rect_this.bottom);
		dc.LineTo(int(degree * rect_this.Width()), rect_this.bottom);
		DeleteObject(&pen);
		ReleaseDC(&dc);
		::SetWindowLong(GetSafeHwnd(), GWL_EXSTYLE, ::GetWindowLongPtr(GetSafeHwnd(), GWL_EXSTYLE) | WS_EX_LAYERED);
		this->SetLayeredWindowAttributes(0, 255 * degree, LWA_ALPHA);
	}
	CDialogEx::OnLButtonDown(nFlags, point);
}

void CVeerShow::OnPaint()
{
	CPaintDC dc(this);
	do
	{
		CRect rect_this;
		GetClientRect(&rect_this);
		CClientDC dc_client(this);
		CPen pen_old(PS_SOLID, 5, RGB(220, 220, 220));
		CPen* p_old_pen = dc_client.SelectObject(&pen_old);
		dc_client.MoveTo(rect_this.left, rect_this.bottom);
		dc_client.LineTo(rect_this.right, rect_this.bottom);

		CPen pen(PS_SOLID, 5, RGB(0, 153, 51));
		CPen* pPen = dc_client.SelectObject(&pen);
		dc_client.MoveTo(rect_this.left, rect_this.bottom);
		dc_client.LineTo(int(degree * rect_this.Width()), rect_this.bottom);

		::SetWindowLong(GetSafeHwnd(), GWL_EXSTYLE, ::GetWindowLongPtr(GetSafeHwnd(), GWL_EXSTYLE) | WS_EX_LAYERED);
		this->SetLayeredWindowAttributes(0, 255 * degree, LWA_ALPHA);
		// 画斜率图
		CPen pen_bkg(PS_SOLID, 10, RGB(240, 240, 240));
		CPen pen_slope(PS_SOLID, 10, RGB(0, 51, 153));
		dc_client.SelectObject(pen_bkg);
		dc_client.MoveTo(rect_this.Width() / 2, rect_this.bottom - rect_this.Height() / 4);
		int length_temp = rect_this.Height() - rect_this.Height() / 4 - 10;
		dc_client.LineTo(rect_this.Width() / 2 - length_temp * sin(last_slope_angle), rect_this.bottom - rect_this.Height() / 4 - length_temp * cos(last_slope_angle));
		if (last_slope_angle > atan(*slope))
			last_slope_angle -= 0.08;
		else if (last_slope_angle < atan(*slope))
			last_slope_angle += 0.08;
		if (abs(last_slope_angle - atan(*slope)) < 0.08)
		{
			last_slope_angle = atan(*slope);
		}
		CPen* p_pen_slope = dc_client.SelectObject(&pen_slope);
		dc_client.MoveTo(rect_this.Width() / 2, rect_this.bottom - rect_this.Height() / 4);
		int length_ = rect_this.Height() - rect_this.Height() / 4 - 10;
		dc_client.LineTo(rect_this.Width() / 2 - length_ * sin(last_slope_angle), rect_this.bottom - rect_this.Height() / 4 - length_ * cos(last_slope_angle));
		CRect rect_edit_slope;
		CWnd* pWnd_edit_slope = GetDlgItem(IDC_STATIC_SLOPE);
		if (pWnd_edit_slope)
		{
			pWnd_edit_slope->GetWindowRect(&rect_edit_slope);
			ScreenToClient(&rect_edit_slope);
			pWnd_edit_slope->SetWindowPos(NULL, rect_this.Width() / 2 - rect_edit_slope.Width() / 2, rect_this.bottom - rect_this.Height() / 4 + 10, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
			CString str_edit_slope;
			str_edit_slope.Format(_T("%lf"), tan(last_slope_angle));
			SetDlgItemTextW(IDC_STATIC_SLOPE, str_edit_slope);
		}

		DeleteObject(&pen_slope);
		DeleteObject(&pen_old);
		DeleteObject(&pen);
		DeleteObject(&pen_bkg);
		ReleaseDC(&dc_client);

		Sleep(8);
	} while (last_slope_angle != atan(*slope));
}

void CVeerShow::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	switch (nIDEvent)
	{
	case ID_FLASH_SLOPE:
	{
		/*CRect rect;
		GetClientRect(&rect);
		InvalidateRect(&rect);*/
		OnPaint();
		break;
	}
	default:
		break;
	}
	CDialogEx::OnTimer(nIDEvent);
}

void CVeerShow::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	CRect rect;
	GetClientRect(&rect);
	InvalidateRect(&rect);
	// TODO: 在此处添加消息处理程序代码
}
