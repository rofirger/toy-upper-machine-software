// CRTwaveform.cpp: 实现文件
//

#include "pch.h"
#include "UpperMachineSoftware.h"
#include "CRTWaveform.h"
#include "afxdialogex.h"
#include <malloc.h>

// CRTwaveform 对话框
bool is_finish_pid = false;
double* _x = new double;
// double* _y = new double;
size_t y_now_index = 0;
size_t y_now_size = 1;
std::list<double*> m_line_serie;

IMPLEMENT_DYNAMIC(CRTwaveform, CDialogEx)

CRTwaveform::CRTwaveform(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_PID, pParent)
{
}

CRTwaveform::~CRTwaveform()
{
}

void CRTwaveform::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_combo_coms);
	DDX_Control(pDX, IDC_COMBO2, m_combo_baudrate);
	DDX_Control(pDX, IDC_COMBO3, m_combo_databit);
	DDX_Control(pDX, IDC_COMBO4, m_combo_check);
	DDX_Control(pDX, IDC_COMBO5, m_combo_stopbit);
	DDX_Control(pDX, IDC_CHECK1, m_check_noerr);
	DDX_Control(pDX, IDC_CHECK2, m_check_reconnect);
	DDX_Control(pDX, IDC_LIST2, m_list_check);
	// DDX_Control(pDX, IDC_LIST_COLOR, m_listbox_color_box);
}

BEGIN_MESSAGE_MAP(CRTwaveform, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON3, &CRTwaveform::OnBnClickedButton3)
	ON_LBN_SELCHANGE(IDC_LIST2, &CRTwaveform::OnLbnSelchangeList)
	ON_WM_TIMER()
	ON_WM_CTLCOLOR()
	ON_WM_SIZE()
END_MESSAGE_MAP()

// CRTwaveform 消息处理程序

BOOL CRTwaveform::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	//m_font.CreateFont(18, 0, 0, 0, FW_SEMIBOLD, false, false, false, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("楷体"));
	ReadCom();

	m_combo_baudrate.AddString(CString(std::string(std::to_string(CBR_110)).c_str()));
	m_combo_baudrate.AddString(CString(std::string(std::to_string(CBR_300)).c_str()));
	m_combo_baudrate.AddString(CString(std::string(std::to_string(CBR_600)).c_str()));
	m_combo_baudrate.AddString(CString(std::string(std::to_string(CBR_1200)).c_str()));
	m_combo_baudrate.AddString(CString(std::string(std::to_string(CBR_2400)).c_str()));
	m_combo_baudrate.AddString(CString(std::string(std::to_string(CBR_4800)).c_str()));
	m_combo_baudrate.AddString(CString(std::string(std::to_string(CBR_9600)).c_str()));
	m_combo_baudrate.AddString(CString(std::string(std::to_string(CBR_14400)).c_str()));
	m_combo_baudrate.AddString(CString(std::string(std::to_string(CBR_19200)).c_str()));
	m_combo_baudrate.AddString(CString(std::string(std::to_string(CBR_38400)).c_str()));
	m_combo_baudrate.AddString(CString(std::string(std::to_string(CBR_56000)).c_str()));
	m_combo_baudrate.AddString(CString(std::string(std::to_string(CBR_57600)).c_str()));
	m_combo_baudrate.AddString(CString(std::string(std::to_string(CBR_115200)).c_str()));
	m_combo_baudrate.AddString(CString(std::string(std::to_string(CBR_128000)).c_str()));
	m_combo_baudrate.AddString(CString(std::string(std::to_string(CBR_256000)).c_str()));
	m_combo_baudrate.SetCurSel(6);

	m_combo_databit.AddString(CString(std::string(std::to_string(5)).c_str()));
	m_combo_databit.AddString(CString(std::string(std::to_string(6)).c_str()));
	m_combo_databit.AddString(CString(std::string(std::to_string(7)).c_str()));
	m_combo_databit.AddString(CString(std::string(std::to_string(8)).c_str()));
	m_combo_databit.SetCurSel(3);

	m_combo_check.AddString(CString("无"));
	m_combo_check.AddString(CString("奇校验"));
	m_combo_check.AddString(CString("偶校验"));
	m_combo_check.SetCurSel(0);

	m_combo_stopbit.AddString(CString(std::string(std::to_string(1)).c_str()));
	m_combo_stopbit.AddString(CString(std::string(std::to_string(2)).c_str()));
	m_combo_stopbit.SetCurSel(0);

	CRect rect;
	// TODO: 在此添加额外的初始化代码

//	m_HSChartCtrl.ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_PIC)->GetWindowRect(rect);
	GetDlgItem(IDC_STATIC_PIC)->ShowWindow(SW_HIDE);
	ScreenToClient(rect);

	m_tab.Create(CMFCTabCtrl::STYLE_3D_ONENOTE,//控件样式，如最上面所演示
		rect,//控件区域
		this,//控件的父窗口指针
		1,//控件ID
		CMFCTabCtrl::LOCATION_TOP);//标签位置
	m_tab.AutoSizeWindow(TRUE);//可以让对话框随tab显示区域而缩放，同时出发对话框的OnSize消息

	m_HSChartCtrl.Create(&m_tab, rect, 2);
	CChartAxis* pAxis = NULL;
	//初始化坐标
	pAxis = m_HSChartCtrl.CreateStandardAxis(CChartCtrl::BottomAxis);
	pAxis->SetAutomatic(true);
	pAxis = m_HSChartCtrl.CreateStandardAxis(CChartCtrl::LeftAxis);
	pAxis->SetAutomatic(true);
	// m_pLineSerie = m_HSChartCtrl.CreateLineSerie();
	UpdateData(FALSE);
	m_tab.AddTab(&m_HSChartCtrl, _T("数据变化波形"));
	m_tab.SetActiveTab(0);//设置显示第一页；
	m_tab.ShowWindow(SW_SHOWNORMAL);
	CRect TabBRect, TabTRect;
	m_tab.GetWindowRect(rect);
	ScreenToClient(rect);
	m_tab.GetTabArea(TabTRect, TabBRect);
	rect.top += TabTRect.Height();

	// test
	/*m_list_check.AddString(_T("曲线88"));
	m_list_check.AddString(_T("曲线9"));
	m_list_check.AddString(_T("曲线90"));*/

	/*m_listbox_color_box.AddString(_T("曲线1"), RGB(255, 0, 0));
	m_listbox_color_box.AddString(_T("曲线2"), RGB(0, 255, 0));
	m_listbox_color_box.AddString(_T("曲线3"), RGB(0, 0, 255));
	m_listbox_color_box.AddString(_T("曲线4"), RGB(255, 0, 255));*/
	/*
	CChartLineSerie* temp;
	temp = m_HSChartCtrl.CreateLineSerie();
	double x_1[100];
	double y_1[100];
	double y_2[100];
	for (int i = 0; i < 100; ++i)
	{
		x_1[i] = i;
		y_1[i] = i;
		y_2[i] = i * 2;
	}
	m_pLineSerie->AddPoints(x_1, y_1, 100);

	temp->AddPoints(x_1, y_2, 100); */
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

void CRTwaveform::ReadCom()
{
	HKEY h_key;
	wchar_t port_name[256];
	UCHAR com_name[256];
	// 打开串口注册表对应的键值  
	if (ERROR_SUCCESS == ::RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("Hardware\\DeviceMap\\SerialComm"), NULL, KEY_READ, &h_key))
	{
		int i = 0;
		int mm = 0;
		DWORD  dw_long, dw_size;
		while (TRUE)
		{
			dw_long = dw_size = sizeof(port_name);
			// 枚举串口
			if (ERROR_NO_MORE_ITEMS == ::RegEnumValue(h_key, i, port_name, &dw_long, NULL, NULL, (PUCHAR)com_name, &dw_size))
			{
				break;
			}
			CString temp;
			temp.Format(_T("%s"), com_name);
			m_combo_coms.AddString(temp);
			i++;
		}
		// 关闭注册表
		RegCloseKey(h_key);
		m_combo_coms.SetCurSel(0);
	}
	else
	{
		rofirger::add_log(rofirger::LOG_LEVEL_FATAL, "FATAL: REGISTRY error: {Hardware\\DeviceMap\\SerialComm} can't be accessed");
		::MessageBox(NULL, _T("注册表错误!"), _T("警告"), 0);
	}
}
void RefleshGram(CRTwaveform* ptr)
{
	if (is_finish_pid)
	{
		return;
	}
	if (y_now_index != 0)
	{
		if (ptr->m_pLineSeries.size() != m_line_serie.size())
		{
			int num_item = ptr->m_list_check.GetCount();
			for (num_item; num_item > 0; --num_item)
			{
				ptr->m_list_check.DeleteString(num_item - 1);
			}
			ptr->m_pLineSeries.clear();
			for (int i = 0; i < m_line_serie.size(); ++i)
			{
				CChartLineSerie* temp = ptr->m_HSChartCtrl.CreateLineSerie();
				ptr->m_pLineSeries.push_back(temp);
				CString str;
				str.Format(_T("曲线%d"), i + 1);
				ptr->m_list_check.AddString(str, temp->GetColor());
				ptr->m_list_check.SetCheck(i, 1);
			}
		}
		// ptr->m_pLineSerie->ClearSerie();
		int i = 0;
		for (auto plist = m_line_serie.begin(); plist != m_line_serie.end(); ++plist, ++i)
		{
			if (ptr->m_list_check.GetCheck(i) == 1)
			{
				ptr->m_pLineSeries[i]->ClearSerie();
				ptr->m_pLineSeries[i]->AddPoints((double*)_x, (double*)*plist, y_now_index);
			}
		}
		// ptr->m_pLineSerie->AddPoints((double*)_x, (double*)_y, y_now_index);
	}
}

void ReconnectPid(CRTwaveform* ptr, int coms, int baudRate, char parity, int dataBit, int stopBit)
{
	while (!ptr->mySerialPort.InitPort(coms, baudRate, parity, dataBit, stopBit))
	{
		Sleep(3);
		if (ptr->IS_STOP_RECONNECT)
		{
			return;
		}
	}
	while (!ptr->mySerialPort.OpenListenThread())
	{
		Sleep(3);
		if (ptr->IS_STOP_RECONNECT)
		{
			return;
		}
	}
	SetTimer(NULL, 1, 50, 0);
}

void CRTwaveform::EnableControlPid(bool true_or_false)
{
	GetDlgItem(IDC_COMBO1)->EnableWindow(true_or_false);
	GetDlgItem(IDC_COMBO2)->EnableWindow(true_or_false);
	GetDlgItem(IDC_COMBO3)->EnableWindow(true_or_false);
	GetDlgItem(IDC_COMBO4)->EnableWindow(true_or_false);
	GetDlgItem(IDC_COMBO5)->EnableWindow(true_or_false);
	GetDlgItem(IDC_CHECK1)->EnableWindow(true_or_false);
	GetDlgItem(IDC_CHECK2)->EnableWindow(true_or_false);
}

void CRTwaveform::OnBnClickedButton3()
{
	// TODO: 在此添加控件通知处理程序代码
	mySerialPort.SetReponseType(PID);

	is_finish_pid = false;

	CString str;
	str.Format(_T("打开串口"));
	CString temp;
	GetDlgItemTextW(IDC_BUTTON3, temp);
	IS_STOP_RECONNECT = false;

	if (temp == str)
	{
		// 清除原有的数据
		for (auto i = m_line_serie.begin(); i != m_line_serie.end(); ++i)
		{
			delete[] * i;
		}
		m_line_serie.clear();
		y_now_index = 0;
		y_now_size = 1;
		delete[]_x;
		_x = new double;

		SetDlgItemTextW(IDC_BUTTON3, _T("关闭串口"));
		CString str1;
		GetDlgItemTextW(IDC_COMBO1, str1);
		int coms = str1[3] - '0';
		GetDlgItemTextW(IDC_COMBO2, str1);
		int baudRate = _ttoi(str1);
		GetDlgItemTextW(IDC_COMBO3, str1);
		int dataBit = _ttoi(str1);
		GetDlgItemTextW(IDC_COMBO5, str1);
		int stopBit = _ttoi(str1);
		char parity = 'N';
		if (m_combo_check.GetCurSel() != 0)
			parity = 'Y';
		int nState1 = ((CButton*)GetDlgItem(IDC_CHECK1))->GetCheck();
		int nState2 = ((CButton*)GetDlgItem(IDC_CHECK2))->GetCheck();

		if (nState2 == BST_CHECKED)
		{
			reconnect_timer.StartTimer(500, std::bind(ReconnectPid, this, coms, baudRate, parity, dataBit, stopBit));
			return;
		}
		EnableControlPid(false);
		if (mySerialPort.InitPort(coms, baudRate, parity, dataBit, stopBit))
		{
			if (!mySerialPort.OpenListenThread())
			{
				::MessageBox(NULL, _T("开启监听线程失败!"), _T("提示"), 0);
				mySerialPort.CloseListenTread();
				SetDlgItemTextW(IDC_BUTTON3, _T("打开串口"));
				EnableControlPid(true);
				return;
			}
			SetTimer(1, 50, 0);
		}
		else if (nState1 != BST_CHECKED)
		{
			::MessageBox(NULL, _T("初始化串口失败!"), _T("提示"), 0);
			mySerialPort.CloseListenTread();
			SetDlgItemTextW(IDC_BUTTON3, _T("打开串口"));
			EnableControlPid(true);
		}
	}
	else
	{
		is_finish_pid = true;
		mySerialPort.Restart();
		/*delete[]_y;
		_y = new double;*/

		KillTimer(1);
		IS_STOP_RECONNECT = true;
		reconnect_timer.Expire();
		SetDlgItemTextW(IDC_BUTTON3, _T("打开串口"));
		EnableControlPid(true);
	}
}

void CRTwaveform::OnLbnSelchangeList()
{
	// TODO: 在此添加控件通知处理程序代码
	int count = m_list_check.GetCount();
	auto plist = m_line_serie.begin();
	for (int i = 0; i < count && plist != m_line_serie.end(); ++plist, i++)
	{
		if (m_list_check.GetCheck(i) == 1)
		{
			m_pLineSeries[i]->ClearSerie();
			m_pLineSeries[i]->AddPoints((double*)_x, (double*)*plist, y_now_index);
		}
		else
		{
			m_pLineSeries[i]->ClearSerie();
		}
	}
}
void CRTwaveform::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	switch (nIDEvent) {
	case 1:
	{
		RefleshGram(this);
		break;
	}
	default:
		break;
	}

	CDialogEx::OnTimer(nIDEvent);
}

HBRUSH CRTwaveform::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	//// TODO:  在此更改 DC 的任何特性
	//if (pWnd->GetDlgCtrlID() == IDC_STATIC_TIPS)
	//{
	//	pDC->SetTextColor(RGB(190, 70, 70));
	//	pDC->SelectObject(m_font);
	//}
	//// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}

int CColorListBox::AddString(LPCTSTR lpszItem, COLORREF itemColor)
{
	// Add the string to the list box
	int nIndex = CListBox::AddString(lpszItem);

	// save color data
	if (nIndex >= 0)
		SetItemData(nIndex, itemColor);

	return nIndex;
}
void CColorListBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// Losing focus ?
	if (lpDrawItemStruct->itemID == -1)
	{
		DrawFocusRect(lpDrawItemStruct->hDC, &lpDrawItemStruct->rcItem);
		return;
	}

	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

	COLORREF clrOld;
	CString sText;

	// get color info from item data
	COLORREF clrNew = (COLORREF)(lpDrawItemStruct->itemData);

	// item selected ?
	if ((lpDrawItemStruct->itemState & ODS_SELECTED) &&
		(lpDrawItemStruct->itemAction & (ODA_SELECT | ODA_DRAWENTIRE)))
	{
		CBrush brush(::GetSysColor(COLOR_HIGHLIGHT));
		pDC->FillRect(&lpDrawItemStruct->rcItem, &brush);
	}

	// item deselected ?
	if (!(lpDrawItemStruct->itemState & ODS_SELECTED) &&
		(lpDrawItemStruct->itemAction & ODA_SELECT))
	{
		CBrush brush(::GetSysColor(COLOR_WINDOW));
		pDC->FillRect(&lpDrawItemStruct->rcItem, &brush);
	}

	// item has focus ?
	if ((lpDrawItemStruct->itemAction & ODA_FOCUS) &&
		(lpDrawItemStruct->itemState & ODS_FOCUS))
	{
		pDC->DrawFocusRect(&lpDrawItemStruct->rcItem);
	}

	// lost focus ?
	if ((lpDrawItemStruct->itemAction & ODA_FOCUS) &&
		!(lpDrawItemStruct->itemState & ODS_FOCUS))
	{
		pDC->DrawFocusRect(&lpDrawItemStruct->rcItem);
	}

	// set the background mode to TRANSPARENT
	int nBkMode = pDC->SetBkMode(TRANSPARENT);

	if (lpDrawItemStruct->itemState & ODS_SELECTED)
		clrOld = pDC->SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
	else
		if (lpDrawItemStruct->itemState & ODS_DISABLED)
			clrOld = pDC->SetTextColor(::GetSysColor(COLOR_GRAYTEXT));
		else
			clrOld = pDC->SetTextColor(clrNew);

	// get item text
	GetText(lpDrawItemStruct->itemID, sText);
	CRect rect = lpDrawItemStruct->rcItem;

	// text format
	UINT nFormat = DT_LEFT | DT_SINGLELINE | DT_VCENTER;
	if (GetStyle() & LBS_USETABSTOPS)
		nFormat |= DT_EXPANDTABS;

	// draw the text
	pDC->DrawText(sText, -1, &rect, nFormat);

	// restore old values
	pDC->SetTextColor(clrOld);
	pDC->SetBkMode(nBkMode);
}
void CColorListBox::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	lpMeasureItemStruct->itemHeight = ::GetSystemMetrics(SM_CYMENUCHECK);
}

void CRTwaveform::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	// 客户区
	CRect client_rect;
	CRect rect_static_border;
	CRect rect_list;
	CRect rect_tips;
	GetClientRect(&client_rect);
	CWnd* pWnd = GetDlgItem(IDC_STATIC_PIC);
	CWnd* pWnd_static_border = GetDlgItem(IDC_STATIC_BORDER);
	CWnd* pWnd_list = GetDlgItem(IDC_LIST2);
	CWnd* pWnd_tips = GetDlgItem(IDC_STATIC_TIPS);

	CWnd* pWnd_coms = GetDlgItem(IDC_COMBO1);
	CWnd* pWnd_baudrate = GetDlgItem(IDC_COMBO2);
	CWnd* pWnd_databits = GetDlgItem(IDC_COMBO3);
	CWnd* pWnd_check = GetDlgItem(IDC_COMBO4);
	CWnd* pWnd_stopbit = GetDlgItem(IDC_COMBO5);
	CWnd* pWnd_noerr = GetDlgItem(IDC_CHECK1);
	CWnd* pWnd_reconnect = GetDlgItem(IDC_CHECK2);
	CWnd* pWnd_static_1 = GetDlgItem(IDC_STATIC_1);
	CWnd* pWnd_static_2 = GetDlgItem(IDC_STATIC_2);
	CWnd* pWnd_static_3 = GetDlgItem(IDC_STATIC_3);
	CWnd* pWnd_static_4 = GetDlgItem(IDC_STATIC_4);
	CWnd* pWnd_static_5 = GetDlgItem(IDC_STATIC_5);
	CWnd* pWnd_button = GetDlgItem(IDC_BUTTON3);
	if (pWnd_static_1 && pWnd_static_2 && pWnd_static_3 && pWnd_static_4 && pWnd_static_5)
	{
		pWnd_static_1->GetWindowRect(&rect_s_1);
		ScreenToClient(&rect_s_1);
		pWnd_static_2->GetWindowRect(&rect_s_2);
		ScreenToClient(&rect_s_2);
		pWnd_static_3->GetWindowRect(&rect_s_3);
		ScreenToClient(&rect_s_3);
		pWnd_static_4->GetWindowRect(&rect_s_4);
		ScreenToClient(&rect_s_4);
		pWnd_static_5->GetWindowRect(&rect_s_5);
		ScreenToClient(&rect_s_5);
		pWnd_coms->GetWindowRect(&rect_coms);
		ScreenToClient(&rect_coms);
		pWnd_baudrate->GetWindowRect(&rect_baudrate);
		ScreenToClient(&rect_baudrate);
		pWnd_databits->GetWindowRect(&rect_databits);
		ScreenToClient(&rect_databits);
		pWnd_check->GetWindowRect(&rect_check);
		ScreenToClient(&rect_check);
		pWnd_stopbit->GetWindowRect(&rect_stopbit);
		ScreenToClient(&rect_stopbit);
		pWnd_noerr->GetWindowRect(&rect_noerr);
		ScreenToClient(&rect_noerr);
		pWnd_reconnect->GetWindowRect(&rect_reconnect);
		ScreenToClient(&rect_reconnect);
		pWnd_button->GetWindowRect(&rect_button);
		ScreenToClient(&rect_button);
	}

	if (pWnd_static_border)
	{
		pWnd_static_border->SetWindowPos(NULL, client_rect.left + 20,
			client_rect.top + 20,
			rect_s_1.Width() + 50 + rect_coms.Width(),
			rect_coms.Height() * 5 + rect_button.Height() + rect_noerr.Height() * 2 + 130,
			SWP_NOZORDER);
		pWnd_static_border->GetWindowRect(&rect_static_border);
		ScreenToClient(&rect_static_border);

		pWnd_static_1->SetWindowPos(NULL, rect_static_border.left + 25, rect_static_border.top + 35, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		pWnd_static_1->GetWindowRect(&rect_s_1);
		ScreenToClient(&rect_s_1);
		pWnd_coms->SetWindowPos(NULL, rect_s_1.right, rect_s_1.top - (rect_coms.Height() - rect_s_1.Height()) / 2, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		pWnd_coms->GetWindowRect(&rect_coms);
		ScreenToClient(&rect_coms);

		pWnd_static_2->SetWindowPos(NULL, rect_static_border.left + 25, rect_s_1.bottom + 15, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		pWnd_static_2->GetWindowRect(&rect_s_2);
		ScreenToClient(&rect_s_2);
		pWnd_baudrate->SetWindowPos(NULL, rect_s_1.right, rect_s_2.top - (rect_baudrate.Height() - rect_s_2.Height()) / 2, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		pWnd_baudrate->GetWindowRect(&rect_baudrate);
		ScreenToClient(&rect_baudrate);

		pWnd_static_3->SetWindowPos(NULL, rect_static_border.left + 25, rect_s_2.bottom + 15, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		pWnd_static_3->GetWindowRect(&rect_s_3);
		ScreenToClient(&rect_s_3);
		pWnd_databits->SetWindowPos(NULL, rect_s_1.right, rect_s_3.top - (rect_databits.Height() - rect_s_3.Height()) / 2, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		pWnd_databits->GetWindowRect(&rect_databits);
		ScreenToClient(&rect_databits);

		pWnd_static_4->SetWindowPos(NULL, rect_static_border.left + 25, rect_s_3.bottom + 15, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		pWnd_static_4->GetWindowRect(&rect_s_4);
		ScreenToClient(&rect_s_4);
		pWnd_check->SetWindowPos(NULL, rect_s_1.right, rect_s_4.top - (rect_check.Height() - rect_s_4.Height()) / 2, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		pWnd_check->GetWindowRect(&rect_check);
		ScreenToClient(&rect_check);

		pWnd_static_5->SetWindowPos(NULL, rect_static_border.left + 25, rect_s_4.bottom + 15, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		pWnd_static_5->GetWindowRect(&rect_s_5);
		ScreenToClient(&rect_s_5);
		pWnd_stopbit->SetWindowPos(NULL, rect_s_1.right, rect_s_5.top - (rect_stopbit.Height() - rect_s_5.Height()) / 2, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		pWnd_stopbit->GetWindowRect(&rect_stopbit);
		ScreenToClient(&rect_stopbit);

		pWnd_button->SetWindowPos(NULL, rect_static_border.left + 40, rect_s_5.bottom + 50, rect_static_border.Width() - 80, rect_coms.Height() + 18, SWP_NOZORDER);
		pWnd_button->GetWindowRect(&rect_button);
		ScreenToClient(&rect_button);
		pWnd_reconnect->SetWindowPos(NULL, (rect_static_border.Width() - rect_reconnect.Width()) / 2,
			rect_static_border.bottom - 12 - rect_reconnect.Height(),
			0, 0, SWP_NOZORDER | SWP_NOSIZE);
		pWnd_reconnect->GetWindowRect(&rect_reconnect);
		ScreenToClient(&rect_reconnect);

		pWnd_noerr->SetWindowPos(NULL, (rect_static_border.Width() - rect_reconnect.Width()) / 2,
			rect_reconnect.top - rect_reconnect.Height() - 4,
			0, 0, SWP_NOZORDER | SWP_NOSIZE);
		pWnd_noerr->GetWindowRect(&rect_noerr);
		ScreenToClient(&rect_noerr);
	}
	if (pWnd_list)
	{
		pWnd_list->SetWindowPos(NULL, rect_static_border.right, rect_static_border.bottom - 20, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		pWnd_list->GetWindowRect(&rect_list);
		ScreenToClient(&rect_list);
	}
	if (pWnd_tips)
	{
		pWnd_tips->SetWindowPos(NULL, rect_list.right + 5, client_rect.top + 10, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		pWnd_tips->GetWindowRect(&rect_tips);
		ScreenToClient(&rect_tips);
	}
	if (pWnd)
	{
		pWnd->SetWindowPos(NULL, rect_list.right, rect_tips.bottom + 5, client_rect.Width() - rect_list.right - 40, client_rect.Height() - rect_tips.bottom - 35, SWP_NOZORDER | SWP_NOMOVE);
		m_tab.SetWindowPos(NULL, rect_list.right, rect_tips.bottom + 5, client_rect.Width() - rect_list.right - 40, client_rect.Height() - rect_tips.bottom - 35, 0);
	}
}
