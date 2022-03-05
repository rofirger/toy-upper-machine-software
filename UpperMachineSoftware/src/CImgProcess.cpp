// 编译dll提醒，务必使用x64对dll进行编译，否则无法使用opencv库

#include "pch.h"
#include "UpperMachineSoftware.h"
#include "CImgProcess.h"
#include "afxdialogex.h"
#include "CSimpleInfo.h"
#include <opencv2/opencv.hpp>
#include <opencv2/core/utils/logger.hpp>
#include <math.h>
#include <fstream>
#include <vector>
#include <afxpriv.h>
#include <exception>
#include "CPixelSrc.h"

// 二值化
#include "binaryzation.h"
// 透视变换
#include "perspective_transform.h"
// 辅助线提取
#include "auxiliary_line.h"
// 用户自定义算法
#include "user_process.h"

using namespace cv;

IMPLEMENT_DYNAMIC(CImgProcess, CDialogEx)

CImgProcess::CImgProcess(CWnd* pParent)
	: CDialogEx(IDD_IMGPROCESS, pParent),
	data_src_param(this, &process_pic_func_param, this, this, EnableControlForDataSrc, PicDataProcess, UpdateCSCCData, ClearCSCCDate, true, raw_pic_data, raw_pic_data_ele_size),
	forms_cam_uart(data_src_param), forms_cam_net_server(data_src_param), forms_cam_net_client(data_src_param)
{
}

CImgProcess::~CImgProcess()
{
	delete file_look;
}
void CImgProcess::MainDlgDropFiles(const std::vector<CString>& main_dlg_drops_files)
{
	for (int i = 0; i < main_dlg_drops_files.size(); ++i)
	{
		drop_files.push_back(main_dlg_drops_files[i]);
	}
}

void CImgProcess::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO6, m_combo_pictype);
	DDX_Control(pDX, IDC_EDIT_WIDTH, m_edit_width);
	DDX_Control(pDX, IDC_EDIT_HEIGTH, m_edit_heigth);
	DDX_Control(pDX, IDC_EDIT_THRESHOLD_VALUE, m_edit_thval);
	DDX_Control(pDX, IDC_SPIN2, m_spin);
	DDX_Control(pDX, IDC_SLIDER1, m_slider_zoom_process);
	DDX_Control(pDX, IDC_SLIDER2, m_slider_zoom_raw);
	DDX_Control(pDX, IDC_SLIDER3, m_slider_zoom_perspective);
	DDX_Control(pDX, IDC_COMBO_ONE, m_combo_process_one);
	DDX_Control(pDX, IDC_COMBO_TWO, m_combo_process_two);
	DDX_Control(pDX, IDC_COMBO_THREE, m_combo_process_three);
	DDX_Control(pDX, IDC_COMBO_FOUR, m_combo_process_four);
	DDX_Control(pDX, IDC_LIST2, m_list_transform);
	DDX_Control(pDX, IDC_EDIT_USER_OUTPUT, m_user_output);
	DDX_Control(pDX, IDC_EDIT_USER_INPUT, m_user_input);
	DDX_Control(pDX, IDC_TAB_MODE, m_tab_mode);
}

BEGIN_MESSAGE_MAP(CImgProcess, CDialogEx)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_NOTIFY(NM_CLICK, IDC_LIST2, &CImgProcess::OnNMClickList2)
	ON_WM_HSCROLL()
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER2, &CImgProcess::OnNMCustomdrawSlider2)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER1, &CImgProcess::OnNMCustomdrawSlider1)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER3, &CImgProcess::OnNMCustomdrawSliderPerspective)
	ON_BN_CLICKED(IDC_SAVE_RAW, &CImgProcess::OnBnClickedSaveRaw)
	ON_BN_CLICKED(IDC_SAVE_PROCESS, &CImgProcess::OnBnClickedSaveProcess)
	ON_BN_CLICKED(IDC_BUTTON1, &CImgProcess::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CImgProcess::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON7, &CImgProcess::OnBnClickedButton7)
	ON_WM_DROPFILES()
	ON_BN_CLICKED(IDC_BUTTON8, &CImgProcess::OnBnClickedButton8)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON_VEER, &CImgProcess::OnBnClickedButtonVeer)
	ON_WM_MOUSEWHEEL()
	ON_BN_CLICKED(IDC_BUTTON_SINGLE_STEP_RIGHT, &CImgProcess::OnBnClickedButtonSingleStepRight)
	ON_BN_CLICKED(IDC_BUTTON_SINGLE_STEP_LEFT, &CImgProcess::OnBnClickedButtonSingleStepLeft)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_MODE, &CImgProcess::OnTcnSelchangeTabMode)
END_MESSAGE_MAP()

// 获取文件夹下的所有文件
void CImgProcess::GetFileFromDirectory(CString csDirPath, std::vector<CString>& vctPath)
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
void CImgProcess::ReadPixelSelectedFile(std::string file_name, std::vector<CString>& vec)
{
	std::fstream  f(file_name);
	std::string file_data;
	while (getline(f, file_data))
	{
		vec.push_back((CString)file_data.c_str());
	}
}

void CImgProcess::WriteOperationInfoToFile()
{
	std::fstream write_file;
	write_file.open("data_", std::ios::out | std::ios::binary);
	CString str_file;
	GetDlgItemTextW(IDC_EDIT_WIDTH, str_file);
	int val_file = _ttoi(str_file);
	write_file.write((char*)&val_file, sizeof(val_file));
	GetDlgItemTextW(IDC_EDIT_HEIGTH, str_file);
	val_file = _ttoi(str_file);
	write_file.write((char*)&val_file, sizeof(val_file));
	GetDlgItemTextW(IDC_EDIT_THRESHOLD_VALUE, str_file);
	val_file = _ttoi(str_file);
	write_file.write((char*)&val_file, sizeof(val_file));
	GetDlgItemTextW(IDC_X_1, str_file);
	val_file = _ttoi(str_file);
	write_file.write((char*)&val_file, sizeof(val_file));
	GetDlgItemTextW(IDC_Y_1, str_file);
	val_file = _ttoi(str_file);
	write_file.write((char*)&val_file, sizeof(val_file));
	GetDlgItemTextW(IDC_X_2, str_file);
	val_file = _ttoi(str_file);
	write_file.write((char*)&val_file, sizeof(val_file));
	GetDlgItemTextW(IDC_Y_2, str_file);
	val_file = _ttoi(str_file);
	write_file.write((char*)&val_file, sizeof(val_file));
	GetDlgItemTextW(IDC_X_3, str_file);
	val_file = _ttoi(str_file);
	write_file.write((char*)&val_file, sizeof(val_file));
	GetDlgItemTextW(IDC_Y_3, str_file);
	val_file = _ttoi(str_file);
	write_file.write((char*)&val_file, sizeof(val_file));
	GetDlgItemTextW(IDC_X_4, str_file);
	val_file = _ttoi(str_file);
	write_file.write((char*)&val_file, sizeof(val_file));
	GetDlgItemTextW(IDC_Y_4, str_file);
	val_file = _ttoi(str_file);
	write_file.write((char*)&val_file, sizeof(val_file));
	GetDlgItemTextW(IDC_X_U_1, str_file);
	val_file = _ttoi(str_file);
	write_file.write((char*)&val_file, sizeof(val_file));
	GetDlgItemTextW(IDC_Y_U_1, str_file);
	val_file = _ttoi(str_file);
	write_file.write((char*)&val_file, sizeof(val_file));
	GetDlgItemTextW(IDC_X_U_2, str_file);
	val_file = _ttoi(str_file);
	write_file.write((char*)&val_file, sizeof(val_file));
	GetDlgItemTextW(IDC_Y_U_2, str_file);
	val_file = _ttoi(str_file);
	write_file.write((char*)&val_file, sizeof(val_file));
	GetDlgItemTextW(IDC_X_U_3, str_file);
	val_file = _ttoi(str_file);
	write_file.write((char*)&val_file, sizeof(val_file));
	GetDlgItemTextW(IDC_Y_U_3, str_file);
	val_file = _ttoi(str_file);
	write_file.write((char*)&val_file, sizeof(val_file));
	GetDlgItemTextW(IDC_X_U_4, str_file);
	val_file = _ttoi(str_file);
	write_file.write((char*)&val_file, sizeof(val_file));
	GetDlgItemTextW(IDC_Y_U_4, str_file);
	val_file = _ttoi(str_file);
	write_file.write((char*)&val_file, sizeof(val_file));

	// 图片处理选择操作
	val_file = m_combo_process_one.GetCurSel();
	write_file.write((char*)&val_file, sizeof(val_file));
	val_file = m_combo_process_two.GetCurSel();
	write_file.write((char*)&val_file, sizeof(val_file));
	val_file = m_combo_process_three.GetCurSel();
	write_file.write((char*)&val_file, sizeof(val_file));
	val_file = m_combo_process_four.GetCurSel();
	write_file.write((char*)&val_file, sizeof(val_file));

	// 图片像素源区
	val_file = is_pixel_src_checked;
	write_file.write((char*)&val_file, sizeof(val_file));
	val_file = interval;
	write_file.write((char*)&val_file, sizeof(val_file));
	write_file.close();

	// 写入像素源文件夹路径
	std::string pixel_src_path_temp_str((CW2A)pixel_src_path.GetString());
	WriteStringToFile("pixel_folder_path.dat", pixel_src_path_temp_str);
}

void CImgProcess::ReadOperationInfoFromFile()
{
	std::fstream read_file;
	read_file.open("data_", std::ios::in | std::ios::binary);
	int val_;
	if (!read_file.read((char*)&val_, sizeof(val_))) val_ = 0;
	SetDlgItemTextW(IDC_EDIT_WIDTH, CString(std::string(std::to_string(val_)).c_str()));
	if (!read_file.read((char*)&val_, sizeof(val_))) val_ = 0;
	SetDlgItemTextW(IDC_EDIT_HEIGTH, CString(std::string(std::to_string(val_)).c_str()));
	if (!read_file.read((char*)&val_, sizeof(val_))) val_ = 0;
	SetDlgItemTextW(IDC_EDIT_THRESHOLD_VALUE, CString(std::string(std::to_string(val_)).c_str()));
	if (!read_file.read((char*)&val_, sizeof(val_))) val_ = 0;
	SetDlgItemTextW(IDC_X_1, CString(std::string(std::to_string(val_)).c_str()));
	if (!read_file.read((char*)&val_, sizeof(val_))) val_ = 0;
	SetDlgItemTextW(IDC_Y_1, CString(std::string(std::to_string(val_)).c_str()));
	if (!read_file.read((char*)&val_, sizeof(val_))) val_ = 0;
	SetDlgItemTextW(IDC_X_2, CString(std::string(std::to_string(val_)).c_str()));
	if (!read_file.read((char*)&val_, sizeof(val_))) val_ = 0;
	SetDlgItemTextW(IDC_Y_2, CString(std::string(std::to_string(val_)).c_str()));
	if (!read_file.read((char*)&val_, sizeof(val_))) val_ = 0;
	SetDlgItemTextW(IDC_X_3, CString(std::string(std::to_string(val_)).c_str()));
	if (!read_file.read((char*)&val_, sizeof(val_))) val_ = 0;
	SetDlgItemTextW(IDC_Y_3, CString(std::string(std::to_string(val_)).c_str()));
	if (!read_file.read((char*)&val_, sizeof(val_))) val_ = 0;
	SetDlgItemTextW(IDC_X_4, CString(std::string(std::to_string(val_)).c_str()));
	if (!read_file.read((char*)&val_, sizeof(val_))) val_ = 0;
	SetDlgItemTextW(IDC_Y_4, CString(std::string(std::to_string(val_)).c_str()));
	if (!read_file.read((char*)&val_, sizeof(val_))) val_ = 0;

	SetDlgItemTextW(IDC_X_U_1, CString(std::string(std::to_string(val_)).c_str()));
	if (!read_file.read((char*)&val_, sizeof(val_))) val_ = 0;
	SetDlgItemTextW(IDC_Y_U_1, CString(std::string(std::to_string(val_)).c_str()));
	if (!read_file.read((char*)&val_, sizeof(val_))) val_ = 0;
	SetDlgItemTextW(IDC_X_U_2, CString(std::string(std::to_string(val_)).c_str()));
	if (!read_file.read((char*)&val_, sizeof(val_))) val_ = 0;
	SetDlgItemTextW(IDC_Y_U_2, CString(std::string(std::to_string(val_)).c_str()));
	if (!read_file.read((char*)&val_, sizeof(val_))) val_ = 0;
	SetDlgItemTextW(IDC_X_U_3, CString(std::string(std::to_string(val_)).c_str()));
	if (!read_file.read((char*)&val_, sizeof(val_))) val_ = 0;
	SetDlgItemTextW(IDC_Y_U_3, CString(std::string(std::to_string(val_)).c_str()));
	if (!read_file.read((char*)&val_, sizeof(val_))) val_ = 0;
	SetDlgItemTextW(IDC_X_U_4, CString(std::string(std::to_string(val_)).c_str()));
	if (!read_file.read((char*)&val_, sizeof(val_))) val_ = 0;
	SetDlgItemTextW(IDC_Y_U_4, CString(std::string(std::to_string(val_)).c_str()));

	// 图片处理选取操作
	if (!read_file.read((char*)&val_, sizeof(val_))) val_ = 0;
	m_combo_process_one.SetCurSel(val_);
	if (!read_file.read((char*)&val_, sizeof(val_))) val_ = 0;
	m_combo_process_two.SetCurSel(val_);
	if (!read_file.read((char*)&val_, sizeof(val_))) val_ = 0;
	m_combo_process_three.SetCurSel(val_);
	if (!read_file.read((char*)&val_, sizeof(val_))) val_ = 0;
	m_combo_process_four.SetCurSel(val_);

	// 图片像素源区
	if (!read_file.read((char*)&val_, sizeof(val_))) val_ = 0;
	is_pixel_src_checked = val_;
	if (!read_file.read((char*)&val_, sizeof(val_))) val_ = 0;
	interval = val_;
	read_file.close();

	// 获取像素源文件夹路径
	std::string pixel_src_path_temp_str;
	GetAllContent("pixel_folder_path.dat", pixel_src_path_temp_str);
	pixel_src_path = pixel_src_path_temp_str.c_str();
	GetFileFromDirectory(pixel_src_path, pixel_src_file_path);
	ReadPixelSelectedFile("pixel_file_selected.dat", pixel_src_file_path_selected);
}

BOOL CImgProcess::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	// 设置tab
	m_tab_mode.InsertItem(0, _T("串口"));
	m_tab_mode.InsertItem(1, _T("TCP服务器"));
	m_tab_mode.InsertItem(2, _T("TCP客户端"));
	forms_cam_uart.Create(IDD_UART, GetDlgItem(IDC_TAB_MODE));
	forms_cam_net_server.Create(IDD_NET_SERVER, GetDlgItem(IDC_TAB_MODE));
	forms_cam_net_client.Create(IDD_NET_CLIENT, GetDlgItem(IDC_TAB_MODE));
	//子窗口大小设置,不设置的话会把tab标题给覆盖掉
	CRect rect_tab;
	m_tab_mode.GetClientRect(rect_tab);
	rect_tab.InflateRect(-1, -28, -2, -2);
	forms_cam_uart.MoveWindow(&rect_tab);     //每一个子窗口大小都要设置
	forms_cam_net_server.MoveWindow(&rect_tab);
	forms_cam_net_client.MoveWindow(&rect_tab);
	//初始状态显示第一个tab页面
	forms_cam_uart.ShowWindow(SW_SHOW);

	m_combo_pictype.AddString(_T("灰度图像"));
	// m_combo_pictype.AddString(_T("彩色图像"));
	m_combo_pictype.SetCurSel(0);

	m_combo_process_one.AddString(_T("NULL"));
	m_combo_process_one.AddString(_T("二值化"));
	m_combo_process_one.AddString(_T("辅助线"));
	m_combo_process_one.AddString(_T("透视变换"));
	m_combo_process_one.AddString(_T("自定义库"));
	m_combo_process_one.SetCurSel(0);

	m_combo_process_two.AddString(_T("NULL"));
	m_combo_process_two.AddString(_T("二值化"));
	m_combo_process_two.AddString(_T("辅助线"));
	m_combo_process_two.AddString(_T("透视变换"));
	m_combo_process_two.AddString(_T("自定义库"));
	m_combo_process_two.SetCurSel(0);

	m_combo_process_three.AddString(_T("NULL"));
	m_combo_process_three.AddString(_T("二值化"));
	m_combo_process_three.AddString(_T("辅助线"));
	m_combo_process_three.AddString(_T("透视变换"));
	m_combo_process_three.AddString(_T("自定义库"));
	m_combo_process_three.SetCurSel(0);

	m_combo_process_four.AddString(_T("NULL"));
	m_combo_process_four.AddString(_T("二值化"));
	m_combo_process_four.AddString(_T("辅助线"));
	m_combo_process_four.AddString(_T("透视变换"));
	m_combo_process_four.AddString(_T("自定义库"));
	m_combo_process_four.SetCurSel(0);

	m_edit_thval.SetLimitText(3);
	m_spin.SetRange32(0, 255);//表示数值只能在0到10内变化
	m_spin.SetBase(10);//设置进制数,只能是10进制和16进制

	// 恢复上次软件操作
	ReadOperationInfoFromFile();

	m_slider_zoom_raw.SetRange(1, 10);
	m_slider_zoom_raw.SetTicFreq(1);
	m_slider_zoom_raw.SetPos(1);

	m_slider_zoom_process.SetRange(1, 10);
	m_slider_zoom_process.SetTicFreq(1);
	m_slider_zoom_process.SetPos(1);

	m_slider_zoom_perspective.SetRange(1, 10);
	m_slider_zoom_perspective.SetTicFreq(1);
	m_slider_zoom_perspective.SetPos(1);

	// tips
	EnableToolTips();
	m_toolTip.Create(this);
	m_toolTip.Activate(true);
	m_toolTip.SetDelayTime(1400);
	m_toolTip.SetTipTextColor(RGB(0, 0, 255));
	m_toolTip.SetTipBkColor(RGB(255, 255, 255));

	m_toolTip.AddTool(GetDlgItem(IDC_X_1), _T("选中编辑框, 并 Ctrl + 左键图片区域 可自动填入坐标"));
	m_toolTip.AddTool(GetDlgItem(IDC_X_2), _T("选中编辑框, 并 Ctrl + 左键图片区域 可自动填入坐标"));
	m_toolTip.AddTool(GetDlgItem(IDC_X_3), _T("选中编辑框, 并 Ctrl + 左键图片区域 可自动填入坐标"));
	m_toolTip.AddTool(GetDlgItem(IDC_X_4), _T("选中编辑框, 并 Ctrl + 左键图片区域 可自动填入坐标"));
	m_toolTip.AddTool(GetDlgItem(IDC_X_U_1), _T("选中编辑框, 并 Ctrl + 左键图片区域 可自动填入坐标"));
	m_toolTip.AddTool(GetDlgItem(IDC_X_U_2), _T("选中编辑框, 并 Ctrl + 左键图片区域 可自动填入坐标"));
	m_toolTip.AddTool(GetDlgItem(IDC_X_U_3), _T("选中编辑框, 并 Ctrl + 左键图片区域 可自动填入坐标"));
	m_toolTip.AddTool(GetDlgItem(IDC_X_U_4), _T("选中编辑框, 并 Ctrl + 左键图片区域 可自动填入坐标"));
	m_toolTip.AddTool(GetDlgItem(IDC_Y_1), _T("选中编辑框, 并 Ctrl + 左键图片区域 可自动填入坐标"));
	m_toolTip.AddTool(GetDlgItem(IDC_Y_2), _T("选中编辑框, 并 Ctrl + 左键图片区域 可自动填入坐标"));
	m_toolTip.AddTool(GetDlgItem(IDC_Y_3), _T("选中编辑框, 并 Ctrl + 左键图片区域 可自动填入坐标"));
	m_toolTip.AddTool(GetDlgItem(IDC_Y_4), _T("选中编辑框, 并 Ctrl + 左键图片区域 可自动填入坐标"));
	m_toolTip.AddTool(GetDlgItem(IDC_Y_U_1), _T("选中编辑框, 并 Ctrl + 左键图片区域 可自动填入坐标"));
	m_toolTip.AddTool(GetDlgItem(IDC_Y_U_2), _T("选中编辑框, 并 Ctrl + 左键图片区域 可自动填入坐标"));
	m_toolTip.AddTool(GetDlgItem(IDC_Y_U_3), _T("选中编辑框, 并 Ctrl + 左键图片区域 可自动填入坐标"));
	m_toolTip.AddTool(GetDlgItem(IDC_Y_U_4), _T("选中编辑框, 并 Ctrl + 左键图片区域 可自动填入坐标"));
	m_toolTip.AddTool(GetDlgItem(IDC_LIST2), _T("左键点击复制透视变换矩阵"));
	m_toolTip.AddTool(GetDlgItem(IDC_SLIDER1), LPSTR_TEXTCALLBACK);
	m_toolTip.AddTool(GetDlgItem(IDC_SLIDER2), LPSTR_TEXTCALLBACK);
	m_toolTip.AddTool(GetDlgItem(IDC_SLIDER3), LPSTR_TEXTCALLBACK);

	// 更新list_control
	m_list_transform.InsertColumn(0, _T("0"), LVCFMT_CENTER, 132);
	m_list_transform.InsertColumn(1, _T("0"), LVCFMT_CENTER, 134);
	m_list_transform.InsertColumn(2, _T("0"), LVCFMT_CENTER, 134);
	m_list_transform.InsertColumn(3, _T("0"), LVCFMT_CENTER, 134);
	m_list_transform.DeleteColumn(0); // 删除第0列
	for (int i = 0; i < 3; ++i)
	{
		int row = m_list_transform.InsertItem(i, _T("0"));
		m_list_transform.SetItemText(row, 0, _T("0"));
		m_list_transform.SetItemText(row, 1, _T("0"));
		m_list_transform.SetItemText(row, 2, _T("0"));
	}
	((CButton*)GetDlgItem(IDC_CHECK_PIXEL_RAW))->SetCheck(BST_CHECKED);
	((CButton*)GetDlgItem(IDC_CHECK_PIXEL_PROCESS))->SetCheck(BST_CHECKED);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}
unsigned __stdcall RefleshPic(LPVOID lpParam)
//void RefleshPic(void* ptr_param)
{
	RefreshPicFuncParam* ptr_param = reinterpret_cast<RefreshPicFuncParam*>(lpParam);
	CImgProcess* ptr = reinterpret_cast<CImgProcess*>(ptr_param->form);
	ptr->num_receive_pics = 0;
	while (!ptr_param->is_stop)
	{
		unsigned char* raw_pixel_data = new unsigned char[ptr->pic_size];
		while (ptr->pic_data.empty())
		{
			Sleep(100);
			if (ptr_param->is_stop)
			{
				return 0;
			}
		}
		ptr->num_receive_pics++;
		CString temp;
		temp.Format(_T("共接收并处理图片:%d"), ptr->num_receive_pics);
		ptr->SetDlgItemText(IDC_STATIC_RECEIVE_PICS, temp);
		int vec_pic_size = ptr->pic_data.front()->GetSize() > ptr->pic_size ? ptr->pic_size : ptr->pic_data.front()->GetSize();
		unsigned char* temp_pic_raw_pixel_data = ptr->pic_data.front()->GetArray();
		for (int i = 0; i < vec_pic_size; ++i)
		{
			raw_pixel_data[i] = temp_pic_raw_pixel_data[i];
		}
		ptr->pic_data.front()->Release();
		delete ptr->pic_data.front();
		ptr->pic_data.pop();

		Mat img(ptr->pic_height, (size_t)(ptr->pic_width), CV_8UC1);
		Mat img_process = img;
		int index = 0;
		for (size_t i = 0; i < ptr->pic_height; i++)
		{
			for (size_t j = 0; j < ptr->pic_width; j++)
			{
				if (index < vec_pic_size)
				{
					img.at<uchar>(i, j) = raw_pixel_data[index++];
				}
			}
		}
		delete[]raw_pixel_data;
		// 原图像
		CRect rect;
		ptr->GetDlgItem(IDC_STATIC_PIC)->GetClientRect(&rect);  // 获取图片控件矩形框
		int is_zoom_pixel_raw = ((CButton*)ptr->GetDlgItem(IDC_CHECK_PIXEL_RAW))->GetCheck();
		int max_zoom_val = rect.Width() / img.cols < rect.Height() / img.rows ? rect.Width() / img.cols : rect.Height() / img.rows;
		ptr->max_zoom = max_zoom_val;
		int zoom_val_raw = ptr->m_slider_zoom_raw.GetPos() < max_zoom_val ? ptr->m_slider_zoom_raw.GetPos() : max_zoom_val;
		ptr->m_zoom_val_raw = zoom_val_raw;
		ptr->m_slider_zoom_raw.SetPos(zoom_val_raw);
		// 若出现图片大小比放置图片的控件大，则选择不显示并返回
		if (rect.Width() < img.cols || rect.Height() < img.rows)
		{
			MessageBox(NULL, _T("窗口过小, 无法正常显示原图像! 如需显示, 请放大窗体大小!"), _T("警告"), 0);
			return 0;
		}

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
		BITMAPINFO_X pBmpInfo;
		memcpy(&pBmpInfo.bmiHeader, &bitInfo, sizeof(BITMAPINFOHEADER));
		//构造灰度图的调色版
		for (int i = 0; i < 256; i++)
		{
			pBmpInfo.bmiColors[i].rgbBlue = i;
			pBmpInfo.bmiColors[i].rgbGreen = i;
			pBmpInfo.bmiColors[i].rgbRed = i;
			pBmpInfo.bmiColors[i].rgbReserved = 0;
		}
		// 保存Onpaint() 使用的矩阵
		ptr->paint_img_raw = img.clone();
		// 首先刷新图片区域
		CRect rect_refresh_raw_img;
		ptr->GetDlgItem(IDC_STATIC_PIC)->GetWindowRect(&rect_refresh_raw_img);
		ptr->ScreenToClient(&rect_refresh_raw_img);
		ptr->InvalidateRect(&rect_refresh_raw_img, true);

		CDC* pDC = ptr->GetDlgItem(IDC_STATIC_PIC)->GetDC();  //获取图片控件DC
		int nWindowW = rect.Width();				                  //获取窗口宽度
		int nWindowH = rect.Height();				                  //获取窗口高度
		int nImageW = img.cols;				//获取图片宽度
		int nImageH = img.rows;				//获取图片高度
		// 更新图片原点坐标
		ptr->ScreenToClient(&rect);
		ptr->m_pos_raw_x_zero = (int)(rect.Width() - nImageW) / 2;
		ptr->m_pos_raw_y_zero = (int)(rect.Height() - nImageH) / 2;
		////绘图
		//::StretchDIBits(
		//	pDC->GetSafeHdc(),
		//	(int)(nWindowW - nImageW) / 2, (int)(nWindowH - nImageH) / 2, img.cols, img.rows,
		//	0, 0, img.cols, img.rows,
		//	img.data,
		//	(BITMAPINFO*)&pBmpInfo,
		//	DIB_RGB_COLORS,
		//	SRCCOPY
		//);
		ptr->ReleaseDC(pDC);  //释放DC

		// 保存原图像
		CString img_("保存摄像头原图");
		CString text_;
		ptr->GetDlgItemTextW(IDC_SAVE_RAW, text_);

		int state_raw = ((CButton*)ptr->GetDlgItem(IDC_SAVE_RAW))->GetCheck();
		if (state_raw == BST_CHECKED)
		{
			if (img_ == text_)
			{
				time_t timep;
				struct tm* param_time;
				time(&timep);
				param_time = localtime(&timep);
				std::string temp_time = DatetimeToString(*param_time);
				std::string filename_param = ptr->path_raw + "\\" + temp_time + "_raw_" + ".jpg";
				cv::imwrite(filename_param, img);
			}
			// 保存像素文件
			else
			{
				time_t timep;
				struct tm* param_time;
				time(&timep);
				param_time = localtime(&timep);
				std::string temp_time = DatetimeToString(*param_time);
				std::string filename_param = ptr->path_raw + "\\" + temp_time + "_raw_.sfp";
				std::fstream write_file;
				write_file.open(filename_param, std::ios::out | std::ios::binary);
				for (int i = 0; i < img.rows; ++i)
				{
					for (int j = 0; j < img.cols; ++j)
					{
						write_file.write((char*)&img.at<uchar>(i, j), sizeof(unsigned char));
					}
				}
				// 写入图片额外信息
				int kind_pic = ptr->m_combo_pictype.GetCurSel();
				write_file.write((char*)&ptr->pic_width, sizeof(int));
				write_file.write((char*)&ptr->pic_height, sizeof(int));
				write_file.write((char*)&kind_pic, sizeof(int));
				write_file.close();
			}
		}

		// 处理图像
		int _cols = img_process.cols;
		int _rows = img_process.rows;
		unsigned char** pixel_mat = new unsigned char* [_rows];
		for (int i = 0; i < _rows; ++i)
		{
			pixel_mat[i] = new unsigned char[_cols];
		}
		for (int i = 0; i < img_process.rows; ++i)
		{
			for (int j = 0; j < img_process.cols; ++j)
			{
				pixel_mat[i][j] = img_process.at<uchar>(i, j);
			}
		}
		// 是否显示辅助线
		bool is_show_left_line = true;
		bool is_show_mid_line = true;
		bool is_show_right_line = true;

		ptr->is_have_user_process = false;

		try
		{
			UserProcessRet user_process_ret;
			// 图像处理函数调用
			switch (ptr->m_combo_process_one.GetCurSel())
			{
			case 1:
			{pixel_mat = ptr->InternalBinaryProcess(pixel_mat, img_process); break; }
			case 2:
			{pixel_mat = ptr->InternalAuxiliaryProcess(pixel_mat, img_process); break; }
			case 3:
			{pixel_mat = ptr->InternalPerspectiveProcess(pixel_mat, img_process, max_zoom_val); break; }
			case 4:
			{
				ptr->is_have_user_process = true;
				user_process_ret = ptr->InternalUserProcess(pixel_mat, img_process, max_zoom_val, is_show_left_line, is_show_mid_line, is_show_right_line);
				pixel_mat = user_process_ret.dst_pixel_mat;
				_rows = user_process_ret.dst_rows;
				_cols = user_process_ret.dst_cols;
				break;
			}
			default:
				break;
			}
			switch (ptr->m_combo_process_two.GetCurSel())
			{
			case 1:
			{pixel_mat = ptr->InternalBinaryProcess(pixel_mat, img_process); break; }
			case 2:
			{pixel_mat = ptr->InternalAuxiliaryProcess(pixel_mat, img_process); break; }
			case 3:
			{pixel_mat = ptr->InternalPerspectiveProcess(pixel_mat, img_process, max_zoom_val); break; }
			case 4:
			{
				ptr->is_have_user_process = true;
				user_process_ret = ptr->InternalUserProcess(pixel_mat, img_process, max_zoom_val, is_show_left_line, is_show_mid_line, is_show_right_line);
				pixel_mat = user_process_ret.dst_pixel_mat;
				_rows = user_process_ret.dst_rows;
				_cols = user_process_ret.dst_cols;
				break;
			}
			default:
				break;
			}
			switch (ptr->m_combo_process_three.GetCurSel())
			{
			case 1:
			{pixel_mat = ptr->InternalBinaryProcess(pixel_mat, img_process); break; }
			case 2:
			{pixel_mat = ptr->InternalAuxiliaryProcess(pixel_mat, img_process); break; }
			case 3:
			{pixel_mat = ptr->InternalPerspectiveProcess(pixel_mat, img_process, max_zoom_val); break; }
			case 4:
			{
				ptr->is_have_user_process = true;
				user_process_ret = ptr->InternalUserProcess(pixel_mat, img_process, max_zoom_val, is_show_left_line, is_show_mid_line, is_show_right_line);
				pixel_mat = user_process_ret.dst_pixel_mat;
				_rows = user_process_ret.dst_rows;
				_cols = user_process_ret.dst_cols;
				break;
			}
			default:
				break;
			}
			switch (ptr->m_combo_process_four.GetCurSel())
			{
			case 1:
			{pixel_mat = ptr->InternalBinaryProcess(pixel_mat, img_process); break; }
			case 2:
			{pixel_mat = ptr->InternalAuxiliaryProcess(pixel_mat, img_process); break; }
			case 3:
			{pixel_mat = ptr->InternalPerspectiveProcess(pixel_mat, img_process, max_zoom_val); break; }
			case 4:
			{
				ptr->is_have_user_process = true;
				user_process_ret = ptr->InternalUserProcess(pixel_mat, img_process, max_zoom_val, is_show_left_line, is_show_mid_line, is_show_right_line);
				pixel_mat = user_process_ret.dst_pixel_mat;
				_rows = user_process_ret.dst_rows;
				_cols = user_process_ret.dst_cols;
				break;
			}
			default:
				break;
			}
		}
		catch (const char* msg)
		{
			CString user_msg(msg);
			::MessageBox(NULL, user_msg, _T("错误提示"), 0);
		}
		catch (std::exception& e)
		{
			CString msg(e.what());
			::MessageBox(NULL, msg, _T("错误提示"), 0);
		}
		catch (MyException& e)
		{
			CString err_msg(e.what());
			::MessageBox(NULL, err_msg, _T("错误提示"), 0);
		}

		// 缩放图片
		int zoom_val_process = ptr->m_slider_zoom_process.GetPos() < max_zoom_val / ptr->m_zoom_val_perspective ? ptr->m_slider_zoom_process.GetPos() : max_zoom_val / ptr->m_zoom_val_perspective;
		ptr->m_slider_zoom_process.SetPos(zoom_val_process);
		ptr->m_zoom_val_process = zoom_val_process;

		int is_zoom_pixel_process = ((CButton*)ptr->GetDlgItem(IDC_CHECK_PIXEL_PROCESS))->GetCheck();
		// 缩放Mat
		if (is_zoom_pixel_process == BST_CHECKED)
		{
			cv::Mat temp = img_process;
			cv::resize(img_process, img_process, cv::Size(img_process.cols * zoom_val_process, img_process.rows * zoom_val_process));
			for (int i = 0; i < img_process.rows; ++i)
			{
				for (int j = 0; j < img_process.cols; ++j)
				{
					img_process.at<uchar>(i, j) = temp.at<uchar>(i / zoom_val_process, j / zoom_val_process);
				}
			}
		}
		else
		{
			cv::resize(img_process, img_process, cv::Size(img_process.cols * zoom_val_process, img_process.rows * zoom_val_process));
		}

		// 保存图像
		int state_process = ((CButton*)ptr->GetDlgItem(IDC_SAVE_PROCESS))->GetCheck();
		if (state_process == BST_CHECKED)
		{
			if (img_ == text_)
			{
				time_t timep;
				struct tm* param_time;
				time(&timep);
				param_time = localtime(&timep);
				std::string temp_time = DatetimeToString(*param_time);
				std::string filename_param = ptr->path_process + "\\" + temp_time + "_process_" + ".jpg";
				cv::imwrite(filename_param, img_process);
			}
			// 保存像素文件
			else
			{
				time_t timep;
				struct tm* param_time;
				time(&timep);
				param_time = localtime(&timep);
				std::string temp_time = DatetimeToString(*param_time);
				std::string filename_param = ptr->path_process + "\\" + temp_time + "_process_.sfp";
				std::fstream write_file;
				write_file.open(filename_param, std::ios::out | std::ios::binary);
				for (int i = 0; i < img_process.rows; ++i)
				{
					for (int j = 0; j < img_process.cols; ++j)
					{
						write_file.write((char*)&img_process.at<uchar>(i, j), sizeof(unsigned char));
					}
				}
				// 写入图片额外信息
				int kind_pic = ptr->m_combo_pictype.GetCurSel();
				write_file.write((char*)&ptr->pic_width, sizeof(int));
				write_file.write((char*)&ptr->pic_height, sizeof(int));
				write_file.write((char*)&kind_pic, sizeof(int));
				write_file.close();
			}
		}
		// 释放像素数组
		for (int i = 0; i < _rows; ++i)
		{
			delete[]pixel_mat[i];
		}
		delete[]pixel_mat;

		CRect rect_;
		ptr->GetDlgItem(IDC_STATIC_PROCESS_PIC)->GetClientRect(&rect_);  // 获取图片控件矩形框

		int pixelBytes_ = img_process.channels() * (img_process.depth() + 1); // 计算一个像素多少个字节
		// 制作bitmapinfo(数据头)
		BITMAPINFO bitInfo_;
		bitInfo_.bmiHeader.biBitCount = 8 * pixelBytes_;
		bitInfo_.bmiHeader.biWidth = img_process.cols;
		bitInfo_.bmiHeader.biHeight = -img_process.rows;   //注意"-"号(正数时倒着绘制)
		bitInfo_.bmiHeader.biPlanes = 1;
		bitInfo_.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bitInfo_.bmiHeader.biCompression = BI_RGB;
		bitInfo_.bmiHeader.biClrImportant = 0;
		bitInfo_.bmiHeader.biClrUsed = 0;
		bitInfo_.bmiHeader.biSizeImage = 0;
		bitInfo_.bmiHeader.biXPelsPerMeter = 0;
		bitInfo_.bmiHeader.biYPelsPerMeter = 0;
		BITMAPINFO_X pBmpInfo_;
		memcpy(&pBmpInfo_.bmiHeader, &bitInfo_, sizeof(BITMAPINFOHEADER));
		//构造灰度图的调色版
		for (int i = 0; i < 256; i++)
		{
			pBmpInfo_.bmiColors[i].rgbBlue = i;
			pBmpInfo_.bmiColors[i].rgbGreen = i;
			pBmpInfo_.bmiColors[i].rgbRed = i;
			pBmpInfo_.bmiColors[i].rgbReserved = 0;
		}
		ptr->paint_img_process = img_process.clone();
		// 首先刷新图片区域
		CRect rect_refresh_process_img;
		ptr->GetDlgItem(IDC_STATIC_PROCESS_PIC)->GetWindowRect(&rect_refresh_process_img);
		ptr->ScreenToClient(&rect_refresh_process_img);
		ptr->InvalidateRect(&rect_refresh_process_img, true);

		//CDC* pDC_ = ptr->GetDlgItem(IDC_STATIC_PROCESS_PIC)->GetDC();  //获取图片控件DC
		//	//绘图
		//::StretchDIBits(
		//	pDC_->GetSafeHdc(),
		//	0, 0, img_process.cols, img_process.rows,
		//	0, 0, img_process.cols, img_process.rows,
		//	img_process.data,
		//	(BITMAPINFO*)&pBmpInfo_,
		//	DIB_RGB_COLORS,
		//	SRCCOPY
		//);
		//ptr->ReleaseDC(pDC_);  //释放DC
		CDC* pDC_ = ptr->GetDlgItem(IDC_STATIC_PROCESS_PIC)->GetDC();  //获取图片控件DC
		int nWindowW_ = rect_.Width();				                  //获取窗口宽度
		int nWindowH_ = rect_.Height();				                  //获取窗口高度
		int nImageW_ = img_process.cols;				//获取图片宽度
		int nImageH_ = img_process.rows;				//获取图片高度
		ptr->ScreenToClient(&rect_);
		ptr->m_pos_process_x_zero = (int)(rect_.Width() - nImageW_) / 2;
		ptr->m_pos_process_y_zero = (int)(rect_.Height() - nImageH_) / 2;
		//绘图
		//::StretchDIBits(
		//	pDC_->GetSafeHdc(),
		//	(int)(nWindowW_ - nImageW_) / 2, (int)(nWindowH_ - nImageH_) / 2, img_process.cols, img_process.rows,
		//	0, 0, img_process.cols, img_process.rows,
		//	img_process.data,
		//	(BITMAPINFO*)&pBmpInfo_,
		//	DIB_RGB_COLORS,
		//	SRCCOPY
		//);

		// RGB服务
		CDC MemDC; //首先定义一个显示设备对象
		CBitmap MemBitmap;//定义一个位图对象
		//随后建立与屏幕显示兼容的内存显示设备
		MemDC.CreateCompatibleDC(NULL);
		MemBitmap.CreateCompatibleBitmap(pDC_, rect_.Width(), rect_.Height());
		CBitmap* pOldBit = MemDC.SelectObject(&MemBitmap);

		if (ptr->is_have_user_process)
		{
			CClientDC user_rgb_dc(ptr);
			for (int i = 0; i < ptr->user_rgb_mat_vec.size(); ++i)
			{
				for (int j = 0; j < ptr->user_rgb_mat_vec[0].size(); ++j)
				{
					if (ptr->user_rgb_mat_vec[i][j].is_show)
					{
						MemDC.SetPixel(rect_.left + ptr->m_pos_process_x_zero + j, rect_.top + ptr->m_pos_process_y_zero + i, RGB(ptr->user_rgb_mat_vec[i][j].r, ptr->user_rgb_mat_vec[i][j].g, ptr->user_rgb_mat_vec[i][j].b));
					}
				}
			}
		}
		pDC_->BitBlt(ptr->m_pos_process_x_zero, ptr->m_pos_process_y_zero, img_process.cols, img_process.rows,
			&MemDC, 0, 0, SRCCOPY);       //绘图完成后的清理
		MemBitmap.DeleteObject();
		MemDC.DeleteDC();

		ptr->ReleaseDC(pDC_);  //释放DC

		// 释放线数组
		if (ptr->left_line != NULL)
		{
			delete[]ptr->left_line;
			ptr->left_line = NULL;
			ptr->perspective_left_line.clear();
		}
		if (ptr->mid_line != NULL)
		{
			delete[]ptr->mid_line;
			ptr->mid_line = NULL;
			ptr->perspective_mid_line.clear();
		}
		if (ptr->right_line != NULL)
		{
			delete[]ptr->right_line;
			ptr->right_line = NULL;
			ptr->perspective_right_line.clear();
		}
	}
	return 0;
}
// 像素源
void RefleshPicPixel(CImgProcess* ptr)
{
	if (ptr->is_start_pixel_src || ptr->is_single_step_pixel_src)
	{
		// 更新 IDC_PIXEL_SRC_PATH 显示
		ptr->SetDlgItemTextW(IDC_PIXEL_SRC_PATH, _T("[") + CString(std::to_string(ptr->pixel_src_file_path_selected_index + 1).c_str()) + _T("] ") + (ptr->pixel_src_path + ptr->pixel_src_file_path_selected[ptr->pixel_src_file_path_selected_index]));
		if (ptr->pic_info[ptr->pixel_src_file_path_selected_index].pic_height == 1 && ptr->pic_info[ptr->pixel_src_file_path_selected_index].pic_width == 1)
		{
			ptr->SetDlgItemTextW(IDC_PIXEL_SRC_PATH, _T("[") + CString(std::to_string(ptr->pixel_src_file_path_selected_index + 1).c_str()) + _T("] ") + _T("有误文件: ") + (ptr->pixel_src_path + ptr->pixel_src_file_path_selected[ptr->pixel_src_file_path_selected_index]));
		}

		unsigned char* raw_pixel_data = new unsigned char[ptr->pic_info[ptr->pixel_src_file_path_selected_index].pic_height * ptr->pic_info[ptr->pixel_src_file_path_selected_index].pic_width];
		std::fstream read_file;
		read_file.open((ptr->pixel_src_path + ptr->pixel_src_file_path_selected[ptr->pixel_src_file_path_selected_index]).GetBuffer(), std::ios::in | std::ios::binary);

		unsigned char val_;
		int i = 0;
		while (read_file.read((char*)&val_, sizeof(val_)))
		{
			raw_pixel_data[i] = val_;
			i++;
			if (i >= ptr->pic_info[ptr->pixel_src_file_path_selected_index].pic_height * ptr->pic_info[ptr->pixel_src_file_path_selected_index].pic_width)
			{
				break;
			}
		}
		read_file.close();
		ptr->pixel_src_file_path_selected[ptr->pixel_src_file_path_selected_index].ReleaseBuffer();

		Mat img(ptr->pic_info[ptr->pixel_src_file_path_selected_index].pic_height, (size_t)ptr->pic_info[ptr->pixel_src_file_path_selected_index].pic_width, CV_8UC1);
		Mat img_process = img;
		int index = 0;
		for (size_t i = 0; i < ptr->pic_info[ptr->pixel_src_file_path_selected_index].pic_height; i++)
		{
			for (size_t j = 0; j < ptr->pic_info[ptr->pixel_src_file_path_selected_index].pic_width; j++)
			{
				if (index < ptr->pic_info[ptr->pixel_src_file_path_selected_index].pic_height * ptr->pic_info[ptr->pixel_src_file_path_selected_index].pic_width)
				{
					img.at<uchar>(i, j) = raw_pixel_data[index++];
				}
			}
		}
		delete[]raw_pixel_data;

		// 原图像
		CRect rect;
		ptr->GetDlgItem(IDC_STATIC_PIC)->GetClientRect(&rect);  // 获取图片控件矩形框
		int is_zoom_pixel_raw = ((CButton*)ptr->GetDlgItem(IDC_CHECK_PIXEL_RAW))->GetCheck();
		int max_zoom_val = rect.Width() / img.cols < rect.Height() / img.rows ? rect.Width() / img.cols : rect.Height() / img.rows;
		ptr->max_zoom = max_zoom_val;
		int zoom_val_raw = ptr->m_slider_zoom_raw.GetPos() < max_zoom_val ? ptr->m_slider_zoom_raw.GetPos() : max_zoom_val;
		ptr->m_zoom_val_raw = zoom_val_raw;
		ptr->m_slider_zoom_raw.SetPos(zoom_val_raw);
		// 若出现图片大小比放置图片的控件大，则选择不显示并返回
		if (rect.Width() < img.cols || rect.Height() < img.rows)
		{
			MessageBox(NULL, _T("窗口过小, 无法正常显示原图像! 如需显示, 请放大窗体大小!"), _T("警告"), 0);
			return;
		}

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
		BITMAPINFO_X pBmpInfo;
		memcpy(&pBmpInfo.bmiHeader, &bitInfo, sizeof(BITMAPINFOHEADER));
		//构造灰度图的调色版
		for (int i = 0; i < 256; i++)
		{
			pBmpInfo.bmiColors[i].rgbBlue = i;
			pBmpInfo.bmiColors[i].rgbGreen = i;
			pBmpInfo.bmiColors[i].rgbRed = i;
			pBmpInfo.bmiColors[i].rgbReserved = 0;
		}
		// 保存Onpaint() 使用的矩阵
		ptr->paint_img_raw = img.clone();
		// 首先刷新图片区域
		CRect rect_refresh_raw_img;
		ptr->GetDlgItem(IDC_STATIC_PIC)->GetWindowRect(&rect_refresh_raw_img);
		ptr->ScreenToClient(&rect_refresh_raw_img);
		ptr->InvalidateRect(&rect_refresh_raw_img, true);

		CDC* pDC = ptr->GetDlgItem(IDC_STATIC_PIC)->GetDC();  //获取图片控件DC
		int nWindowW = rect.Width();				                  //获取窗口宽度
		int nWindowH = rect.Height();				                  //获取窗口高度
		int nImageW = img.cols;				//获取图片宽度
		int nImageH = img.rows;				//获取图片高度

		// 更新图片原点坐标
		ptr->ScreenToClient(&rect);
		ptr->m_pos_raw_x_zero = (int)(rect.Width() - nImageW) / 2;
		ptr->m_pos_raw_y_zero = (int)(rect.Height() - nImageH) / 2;
		//绘图
		//::StretchDIBits(
		//	pDC->GetSafeHdc(),
		//	(int)(nWindowW - nImageW) / 2, (int)(nWindowH - nImageH) / 2, img.cols, img.rows,
		//	0, 0, img.cols, img.rows,
		//	img.data,
		//	(BITMAPINFO*)&pBmpInfo,
		//	DIB_RGB_COLORS,
		//	SRCCOPY
		//);
		ptr->ReleaseDC(pDC);  //释放DC

		// 保存原图像
		CString img_("保存摄像头原图");
		CString text_;
		ptr->GetDlgItemTextW(IDC_SAVE_RAW, text_);

		int state_raw = ((CButton*)ptr->GetDlgItem(IDC_SAVE_RAW))->GetCheck();
		if (state_raw == BST_CHECKED)
		{
			if (img_ == text_)
			{
				time_t timep;
				struct tm* param_time;
				time(&timep);
				param_time = localtime(&timep);
				std::string temp_time = DatetimeToString(*param_time);
				std::string filename_param = ptr->path_raw + "\\" + temp_time + "_raw_" + ".jpg";
				cv::imwrite(filename_param, img);
			}
			// 保存像素文件
			else
			{
				time_t timep;
				struct tm* param_time;
				time(&timep);
				param_time = localtime(&timep);
				std::string temp_time = DatetimeToString(*param_time);
				std::string filename_param = ptr->path_raw + "\\" + temp_time + "_raw_.sfp";
				std::fstream write_file;
				write_file.open(filename_param, std::ios::out | std::ios::binary);
				for (int i = 0; i < img.rows; ++i)
				{
					for (int j = 0; j < img.cols; ++j)
					{
						write_file.write((char*)&img.at<uchar>(i, j), sizeof(unsigned char));
					}
				}
				// 写入图片额外信息
				write_file.write((char*)&ptr->pic_info[ptr->pixel_src_file_path_selected_index].pic_width, sizeof(int));
				write_file.write((char*)&ptr->pic_info[ptr->pixel_src_file_path_selected_index].pic_height, sizeof(int));
				write_file.write((char*)&ptr->pic_info[ptr->pixel_src_file_path_selected_index].pic_kind, sizeof(int));
				write_file.close();
			}
		}

		// 处理图像
		int _cols = img_process.cols;
		int _rows = img_process.rows;
		unsigned char** pixel_mat = new unsigned char* [_rows];
		for (int i = 0; i < _rows; ++i)
		{
			pixel_mat[i] = new unsigned char[_cols];
		}
		for (int i = 0; i < img_process.rows; ++i)
		{
			for (int j = 0; j < img_process.cols; ++j)
			{
				pixel_mat[i][j] = img_process.at<uchar>(i, j);
			}
		}
		// 是否显示辅助线
		bool is_show_left_line = true;
		bool is_show_mid_line = true;
		bool is_show_right_line = true;
		ptr->is_have_user_process = false;
		try
		{
			UserProcessRet user_process_ret;
			// 图像处理函数调用
			switch (ptr->m_combo_process_one.GetCurSel())
			{
			case 1:
			{pixel_mat = ptr->InternalBinaryProcess(pixel_mat, img_process); break; }
			case 2:
			{pixel_mat = ptr->InternalAuxiliaryProcess(pixel_mat, img_process); break; }
			case 3:
			{pixel_mat = ptr->InternalPerspectiveProcess(pixel_mat, img_process, max_zoom_val); break; }
			case 4:
			{
				ptr->is_have_user_process = true;
				user_process_ret = ptr->InternalUserProcess(pixel_mat, img_process, max_zoom_val, is_show_left_line, is_show_mid_line, is_show_right_line);
				pixel_mat = user_process_ret.dst_pixel_mat;
				_rows = user_process_ret.dst_rows;
				_cols = user_process_ret.dst_cols;
				break;
			}
			default:
				break;
			}
			switch (ptr->m_combo_process_two.GetCurSel())
			{
			case 1:
			{pixel_mat = ptr->InternalBinaryProcess(pixel_mat, img_process); break; }
			case 2:
			{pixel_mat = ptr->InternalAuxiliaryProcess(pixel_mat, img_process); break; }
			case 3:
			{pixel_mat = ptr->InternalPerspectiveProcess(pixel_mat, img_process, max_zoom_val); break; }
			case 4:
			{
				ptr->is_have_user_process = true;
				user_process_ret = ptr->InternalUserProcess(pixel_mat, img_process, max_zoom_val, is_show_left_line, is_show_mid_line, is_show_right_line);
				pixel_mat = user_process_ret.dst_pixel_mat;
				_rows = user_process_ret.dst_rows;
				_cols = user_process_ret.dst_cols;
				break;
			}
			default:
				break;
			}
			switch (ptr->m_combo_process_three.GetCurSel())
			{
			case 1:
			{pixel_mat = ptr->InternalBinaryProcess(pixel_mat, img_process); break; }
			case 2:
			{pixel_mat = ptr->InternalAuxiliaryProcess(pixel_mat, img_process); break; }
			case 3:
			{pixel_mat = ptr->InternalPerspectiveProcess(pixel_mat, img_process, max_zoom_val); break; }
			case 4:
			{
				ptr->is_have_user_process = true;
				user_process_ret = ptr->InternalUserProcess(pixel_mat, img_process, max_zoom_val, is_show_left_line, is_show_mid_line, is_show_right_line);
				pixel_mat = user_process_ret.dst_pixel_mat;
				_rows = user_process_ret.dst_rows;
				_cols = user_process_ret.dst_cols;
				break;
			}
			default:
				break;
			}
			switch (ptr->m_combo_process_four.GetCurSel())
			{
			case 1:
			{pixel_mat = ptr->InternalBinaryProcess(pixel_mat, img_process); break; }
			case 2:
			{pixel_mat = ptr->InternalAuxiliaryProcess(pixel_mat, img_process); break; }
			case 3:
			{pixel_mat = ptr->InternalPerspectiveProcess(pixel_mat, img_process, max_zoom_val); break; }
			case 4:
			{
				ptr->is_have_user_process = true;
				user_process_ret = ptr->InternalUserProcess(pixel_mat, img_process, max_zoom_val, is_show_left_line, is_show_mid_line, is_show_right_line);
				pixel_mat = user_process_ret.dst_pixel_mat;
				_rows = user_process_ret.dst_rows;
				_cols = user_process_ret.dst_cols;
				break;
			}
			default:
				break;
			}
		}
		catch (const char* msg)
		{
			CString user_msg(msg);
			::MessageBox(NULL, user_msg, _T("错误提示"), 0);
		}
		catch (std::exception& e)
		{
			CString msg(e.what());
			::MessageBox(NULL, msg, _T("错误提示"), 0);
		}
		catch (MyException& e)
		{
			CString err_msg(e.what());
			::MessageBox(NULL, err_msg, _T("错误提示"), 0);
		}
		// 辅助线显示
		rofirger::add_log(rofirger::LOG_LEVEL_INFO, "using left_line, mid_line and right_line arrays");
		if (is_show_left_line && ptr->left_line != NULL)
		{
			if (ptr->perspective_left_line.size() != 0)
			{
				for (int i = 0; i < img_process.rows / ptr->m_zoom_val_perspective; ++i)
				{
					img_process.at<uchar>(ptr->perspective_left_line[i].y, ptr->perspective_left_line[i].x) = 0;
				}
			}
			else
			{
				for (int i = 0; i < img_process.rows / ptr->m_zoom_val_perspective; ++i)
				{
					img_process.at<uchar>(i, ptr->left_line[i]) = 0;
				}
			}
		}
		if (is_show_mid_line && ptr->mid_line != NULL)
		{
			if (ptr->perspective_mid_line.size() != 0)
			{
				for (int i = 0; i < img_process.rows / ptr->m_zoom_val_perspective; ++i)
				{
					img_process.at<uchar>(ptr->perspective_mid_line[i].y, ptr->perspective_mid_line[i].x) = 0;
				}
			}
			else
			{
				for (int i = 0; i < img_process.rows / ptr->m_zoom_val_perspective; ++i)
				{
					img_process.at<uchar>(i, ptr->mid_line[i]) = 0;
				}
			}
		}
		if (is_show_right_line && ptr->right_line != NULL)
		{
			if (ptr->perspective_right_line.size() != 0)
			{
				for (int i = 0; i < img_process.rows / ptr->m_zoom_val_perspective; ++i)
				{
					img_process.at<uchar>(ptr->perspective_right_line[i].y, ptr->perspective_right_line[i].x) = 0;
				}
			}
			else
			{
				for (int i = 0; i < img_process.rows / ptr->m_zoom_val_perspective; ++i)
				{
					img_process.at<uchar>(i, ptr->right_line[i]) = 0;
				}
			}
		}

		// 缩放图片
		int zoom_val_process = ptr->m_slider_zoom_process.GetPos() < max_zoom_val / ptr->m_zoom_val_perspective ? ptr->m_slider_zoom_process.GetPos() : max_zoom_val / ptr->m_zoom_val_perspective;
		ptr->m_slider_zoom_process.SetPos(zoom_val_process);
		ptr->m_zoom_val_process = zoom_val_process;

		int is_zoom_pixel_process = ((CButton*)ptr->GetDlgItem(IDC_CHECK_PIXEL_PROCESS))->GetCheck();
		// 缩放Mat
		if (is_zoom_pixel_process == BST_CHECKED)
		{
			cv::Mat temp = img_process;
			cv::resize(img_process, img_process, cv::Size(img_process.cols * zoom_val_process, img_process.rows * zoom_val_process));
			for (int i = 0; i < img_process.rows; ++i)
			{
				for (int j = 0; j < img_process.cols; ++j)
				{
					img_process.at<uchar>(i, j) = temp.at<uchar>(i / zoom_val_process, j / zoom_val_process);
				}
			}
		}
		else
		{
			cv::resize(img_process, img_process, cv::Size(img_process.cols * zoom_val_process, img_process.rows * zoom_val_process));
		}

		// 保存图像
		int state_process = ((CButton*)ptr->GetDlgItem(IDC_SAVE_PROCESS))->GetCheck();
		if (state_process == BST_CHECKED)
		{
			if (img_ == text_)
			{
				time_t timep;
				struct tm* param_time;
				time(&timep);
				param_time = localtime(&timep);
				std::string temp_time = DatetimeToString(*param_time);
				std::string filename_param = ptr->path_process + "\\" + temp_time + "_process_" + ".jpg";
				cv::imwrite(filename_param, img_process);
			}
			// 保存像素文件
			else
			{
				time_t timep;
				struct tm* param_time;
				time(&timep);
				param_time = localtime(&timep);
				std::string temp_time = DatetimeToString(*param_time);
				std::string filename_param = ptr->path_process + "\\" + temp_time + "_process_.sfp";
				std::fstream write_file;
				write_file.open(filename_param, std::ios::out | std::ios::binary);
				for (int i = 0; i < img_process.rows; ++i)
				{
					for (int j = 0; j < img_process.cols; ++j)
					{
						write_file.write((char*)&img_process.at<uchar>(i, j), sizeof(unsigned char));
					}
				}
				// 写入图片额外信息
				write_file.write((char*)&ptr->pic_info[ptr->pixel_src_file_path_selected_index].pic_width, sizeof(int));
				write_file.write((char*)&ptr->pic_info[ptr->pixel_src_file_path_selected_index].pic_height, sizeof(int));
				write_file.write((char*)&ptr->pic_info[ptr->pixel_src_file_path_selected_index].pic_kind, sizeof(int));
				write_file.close();
			}
		}
		// 释放像素数组
		for (int i = 0; i < _rows; ++i)
		{
			delete[]pixel_mat[i];
		}
		delete[]pixel_mat;

		CRect rect_;
		ptr->GetDlgItem(IDC_STATIC_PROCESS_PIC)->GetClientRect(&rect_);  // 获取图片控件矩形框

		int pixelBytes_ = img_process.channels() * (img_process.depth() + 1); // 计算一个像素多少个字节
		// 制作bitmapinfo(数据头)
		BITMAPINFO bitInfo_;
		bitInfo_.bmiHeader.biBitCount = 8 * pixelBytes_;
		bitInfo_.bmiHeader.biWidth = img_process.cols;
		bitInfo_.bmiHeader.biHeight = -img_process.rows;   //注意"-"号(正数时倒着绘制)
		bitInfo_.bmiHeader.biPlanes = 1;
		bitInfo_.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bitInfo_.bmiHeader.biCompression = BI_RGB;
		bitInfo_.bmiHeader.biClrImportant = 0;
		bitInfo_.bmiHeader.biClrUsed = 0;
		bitInfo_.bmiHeader.biSizeImage = 0;
		bitInfo_.bmiHeader.biXPelsPerMeter = 0;
		bitInfo_.bmiHeader.biYPelsPerMeter = 0;
		BITMAPINFO_X pBmpInfo_;
		memcpy(&pBmpInfo_.bmiHeader, &bitInfo_, sizeof(BITMAPINFOHEADER));
		//构造灰度图的调色版
		for (int i = 0; i < 256; i++)
		{
			pBmpInfo_.bmiColors[i].rgbBlue = i;
			pBmpInfo_.bmiColors[i].rgbGreen = i;
			pBmpInfo_.bmiColors[i].rgbRed = i;
			pBmpInfo_.bmiColors[i].rgbReserved = 0;
		}
		ptr->paint_img_process = img_process.clone();
		// 首先刷新图片区域
		CRect rect_refresh_process_img;
		ptr->GetDlgItem(IDC_STATIC_PROCESS_PIC)->GetWindowRect(&rect_refresh_process_img);
		ptr->ScreenToClient(&rect_refresh_process_img);
		ptr->InvalidateRect(&rect_refresh_process_img, true);

		CDC* pDC_ = ptr->GetDlgItem(IDC_STATIC_PROCESS_PIC)->GetDC();  //获取图片控件DC
		int nWindowW_ = rect_.Width();				//获取窗口宽度
		int nWindowH_ = rect_.Height();				//获取窗口高度
		int nImageW_ = img_process.cols;				//获取图片宽度
		int nImageH_ = img_process.rows;				//获取图片高度
		ptr->ScreenToClient(&rect);
		ptr->m_pos_process_x_zero = (int)(rect_.Width() - nImageW_) / 2;
		ptr->m_pos_process_y_zero = (int)(rect_.Height() - nImageH_) / 2;
		//绘图
		//::StretchDIBits(
		//	pDC_->GetSafeHdc(),
		//	(int)(nWindowW_ - nImageW_) / 2, (int)(nWindowH_ - nImageH_) / 2, img_process.cols, img_process.rows,
		//	0, 0, img_process.cols, img_process.rows,
		//	img_process.data,
		//	(BITMAPINFO*)&pBmpInfo_,
		//	DIB_RGB_COLORS,
		//	SRCCOPY
		//);
		//// RGB服务
		////CDC MemDC; //首先定义一个显示设备对象
		////CBitmap MemBitmap;//定义一个位图对象
		//////随后建立与屏幕显示兼容的内存显示设备
		////MemDC.CreateCompatibleDC(NULL);
		////MemBitmap.CreateCompatibleBitmap(pDC_, rect_.Width(), rect_.Height());
		////CBitmap* pOldBit = MemDC.SelectObject(&MemBitmap);
		////pDC_->BitBlt(ptr->m_pos_process_x_zero, ptr->m_pos_process_y_zero, img_process.cols, img_process.rows,
		////	&MemDC, 0, 0, SRCCOPY);       //绘图完成后的清理
		//if (ptr->is_have_user_process)
		//{
		//	CClientDC user_rgb_dc(ptr);
		//	for (int i = 0; i < ptr->user_rgb_mat_vec.size(); ++i)
		//	{
		//		for (int j = 0; j < ptr->user_rgb_mat_vec[0].size(); ++j)
		//		{
		//			if (ptr->user_rgb_mat_vec[i][j].is_show)
		//			{
		//				pDC_->SetPixel(ptr->m_pos_process_x_zero + j, ptr->m_pos_process_y_zero + i, RGB(ptr->user_rgb_mat_vec[i][j].r, ptr->user_rgb_mat_vec[i][j].g, ptr->user_rgb_mat_vec[i][j].b));
		//			}
		//			pDC_->SetPixel(ptr->m_pos_process_x_zero + j, ptr->m_pos_process_y_zero + i, RGB(100, 100, 100));

		//		}
		//	}
		//}
		////pDC_->BitBlt(ptr->m_pos_process_x_zero, ptr->m_pos_process_y_zero, img_process.cols, img_process.rows,
		////	&MemDC, 0, 0, SRCCOPY);       //绘图完成后的清理
		////MemBitmap.DeleteObject();
		////MemDC.DeleteDC();

		//ptr->ReleaseDC(pDC_);  //释放DC
		// 释放线数组
		rofirger::add_log(rofirger::LOG_LEVEL_INFO, "free left_line, mid_line and right_line arrays");
		if (ptr->left_line != NULL)
		{
			delete[]ptr->left_line;
			ptr->left_line = NULL;
			ptr->perspective_left_line.clear();
		}
		if (ptr->mid_line != NULL)
		{
			delete[]ptr->mid_line;
			ptr->mid_line = NULL;
			ptr->perspective_mid_line.clear();
		}
		if (ptr->right_line != NULL)
		{
			delete[]ptr->right_line;
			ptr->right_line = NULL;
			ptr->perspective_right_line.clear();
		}
		if (ptr->is_single_step_pixel_src == false)
		{
			ptr->pixel_src_file_path_selected_index++;
		}
		if (ptr->pixel_src_file_path_selected_index >= ptr->pixel_src_file_path_selected.size())
		{
			ptr->is_start_pixel_src = false;
			ptr->pixel_src_file_path_selected_index = 0;
			ptr->SetDlgItemText(IDC_BUTTON7, _T("开始"));
			EnableControlForPixelSrc(ptr, true);
			::MessageBox(NULL, _T("资源均已被加载！"), _T("提示"), 0);
			// 更新 IDC_PIXEL_SRC_PATH 显示
			ptr->SetDlgItemTextW(IDC_PIXEL_SRC_PATH, _T(""));
			return;
		}
	}
}

cv::Mat& CImgProcess::GetPerspectiveTransformMat()
{
	CString str_x_1;
	CString str_y_1;
	CString str_x_2;
	CString str_y_2;
	CString str_x_3;
	CString str_y_3;
	CString str_x_4;
	CString str_y_4;

	CString str_x_u_1;
	CString str_y_u_1;
	CString str_x_u_2;
	CString str_y_u_2;
	CString str_x_u_3;
	CString str_y_u_3;
	CString str_x_u_4;
	CString str_y_u_4;
	GetDlgItem(IDC_X_1)->GetWindowText(str_x_1);
	GetDlgItem(IDC_Y_1)->GetWindowText(str_y_1);
	GetDlgItem(IDC_X_2)->GetWindowText(str_x_2);
	GetDlgItem(IDC_Y_2)->GetWindowText(str_y_2);
	GetDlgItem(IDC_X_3)->GetWindowText(str_x_3);
	GetDlgItem(IDC_Y_3)->GetWindowText(str_y_3);
	GetDlgItem(IDC_X_4)->GetWindowText(str_x_4);
	GetDlgItem(IDC_Y_4)->GetWindowText(str_y_4);

	GetDlgItem(IDC_X_U_1)->GetWindowText(str_x_u_1);
	GetDlgItem(IDC_Y_U_1)->GetWindowText(str_y_u_1);
	GetDlgItem(IDC_X_U_2)->GetWindowText(str_x_u_2);
	GetDlgItem(IDC_Y_U_2)->GetWindowText(str_y_u_2);
	GetDlgItem(IDC_X_U_3)->GetWindowText(str_x_u_3);
	GetDlgItem(IDC_Y_U_3)->GetWindowText(str_y_u_3);
	GetDlgItem(IDC_X_U_4)->GetWindowText(str_x_u_4);
	GetDlgItem(IDC_Y_U_4)->GetWindowText(str_y_u_4);
	cv::Point2f src_points[] = {
		cv::Point2f(_ttoi(str_x_1), _ttoi(str_y_1)),
		cv::Point2f(_ttoi(str_x_2), _ttoi(str_y_2)),
		cv::Point2f(_ttoi(str_x_3), _ttoi(str_y_3)),
		cv::Point2f(_ttoi(str_x_4), _ttoi(str_y_4)) };

	cv::Point2f dst_points[] = {
		cv::Point2f(_ttoi(str_x_u_1), _ttoi(str_y_u_1)),
		cv::Point2f(_ttoi(str_x_u_2), _ttoi(str_y_u_2)),
		cv::Point2f(_ttoi(str_x_u_3), _ttoi(str_y_u_3)),
		cv::Point2f(_ttoi(str_x_u_4), _ttoi(str_y_u_4)) };
	perspective_transform_mat = cv::getPerspectiveTransform(src_points, dst_points);
	return perspective_transform_mat;
}
void EnableControlForDataSrc(void* ptr_param_cscc, bool true_or_false)
{
	CImgProcess* ptr = reinterpret_cast<CImgProcess*>(ptr_param_cscc);
	ptr->GetDlgItem(IDC_X_1)->EnableWindow(true_or_false);
	ptr->GetDlgItem(IDC_X_2)->EnableWindow(true_or_false);
	ptr->GetDlgItem(IDC_X_3)->EnableWindow(true_or_false);
	ptr->GetDlgItem(IDC_X_3)->EnableWindow(true_or_false);
	ptr->GetDlgItem(IDC_X_4)->EnableWindow(true_or_false);
	ptr->GetDlgItem(IDC_X_U_1)->EnableWindow(true_or_false);
	ptr->GetDlgItem(IDC_X_U_2)->EnableWindow(true_or_false);
	ptr->GetDlgItem(IDC_X_U_3)->EnableWindow(true_or_false);
	ptr->GetDlgItem(IDC_X_U_4)->EnableWindow(true_or_false);

	ptr->GetDlgItem(IDC_Y_1)->EnableWindow(true_or_false);
	ptr->GetDlgItem(IDC_Y_2)->EnableWindow(true_or_false);
	ptr->GetDlgItem(IDC_Y_3)->EnableWindow(true_or_false);
	ptr->GetDlgItem(IDC_Y_3)->EnableWindow(true_or_false);
	ptr->GetDlgItem(IDC_Y_4)->EnableWindow(true_or_false);
	ptr->GetDlgItem(IDC_Y_U_1)->EnableWindow(true_or_false);
	ptr->GetDlgItem(IDC_Y_U_2)->EnableWindow(true_or_false);
	ptr->GetDlgItem(IDC_Y_U_3)->EnableWindow(true_or_false);
	ptr->GetDlgItem(IDC_Y_U_4)->EnableWindow(true_or_false);

	ptr->GetDlgItem(IDC_EDIT_HEIGTH)->EnableWindow(true_or_false);
	ptr->GetDlgItem(IDC_EDIT_WIDTH)->EnableWindow(true_or_false);
	ptr->GetDlgItem(IDC_COMBO6)->EnableWindow(true_or_false);

	ptr->GetDlgItem(IDC_BUTTON2)->EnableWindow(true_or_false);
	ptr->GetDlgItem(IDC_BUTTON7)->EnableWindow(true_or_false);
	ptr->GetDlgItem(IDC_BUTTON8)->EnableWindow(true_or_false);
	ptr->GetDlgItem(IDC_BUTTON_SINGLE_STEP_LEFT)->EnableWindow(true_or_false);
	ptr->GetDlgItem(IDC_BUTTON_SINGLE_STEP_RIGHT)->EnableWindow(true_or_false);
}
void EnableControlForPixelSrc(void* ptr_param_cscc, bool true_or_false)
{
	CImgProcess* ptr = reinterpret_cast<CImgProcess*>(ptr_param_cscc);
	ptr->GetDlgItem(IDC_X_1)->EnableWindow(true_or_false);
	ptr->GetDlgItem(IDC_X_2)->EnableWindow(true_or_false);
	ptr->GetDlgItem(IDC_X_3)->EnableWindow(true_or_false);
	ptr->GetDlgItem(IDC_X_3)->EnableWindow(true_or_false);
	ptr->GetDlgItem(IDC_X_4)->EnableWindow(true_or_false);
	ptr->GetDlgItem(IDC_X_U_1)->EnableWindow(true_or_false);
	ptr->GetDlgItem(IDC_X_U_2)->EnableWindow(true_or_false);
	ptr->GetDlgItem(IDC_X_U_3)->EnableWindow(true_or_false);
	ptr->GetDlgItem(IDC_X_U_4)->EnableWindow(true_or_false);

	ptr->GetDlgItem(IDC_Y_1)->EnableWindow(true_or_false);
	ptr->GetDlgItem(IDC_Y_2)->EnableWindow(true_or_false);
	ptr->GetDlgItem(IDC_Y_3)->EnableWindow(true_or_false);
	ptr->GetDlgItem(IDC_Y_3)->EnableWindow(true_or_false);
	ptr->GetDlgItem(IDC_Y_4)->EnableWindow(true_or_false);
	ptr->GetDlgItem(IDC_Y_U_1)->EnableWindow(true_or_false);
	ptr->GetDlgItem(IDC_Y_U_2)->EnableWindow(true_or_false);
	ptr->GetDlgItem(IDC_Y_U_3)->EnableWindow(true_or_false);
	ptr->GetDlgItem(IDC_Y_U_4)->EnableWindow(true_or_false);

	ptr->GetDlgItem(IDC_EDIT_HEIGTH)->EnableWindow(true_or_false);
	ptr->GetDlgItem(IDC_EDIT_WIDTH)->EnableWindow(true_or_false);
	ptr->GetDlgItem(IDC_COMBO6)->EnableWindow(true_or_false);

	ptr->GetDlgItem(IDC_BUTTON2)->EnableWindow(true_or_false);
	ptr->GetDlgItem(IDC_BUTTON8)->EnableWindow(true_or_false);
	ptr->GetDlgItem(IDC_BUTTON_SINGLE_STEP_LEFT)->EnableWindow(true_or_false);
	ptr->GetDlgItem(IDC_BUTTON_SINGLE_STEP_RIGHT)->EnableWindow(true_or_false);
}

void CImgProcess::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CRect rect_raw;
	GetDlgItem(IDC_STATIC_PIC)->GetWindowRect(&rect_raw); //获取被选中的控件大小
	ScreenToClient(&rect_raw);  //转化为客户区坐标

	CRect rect_process;
	GetDlgItem(IDC_STATIC_PROCESS_PIC)->GetWindowRect(&rect_process); //获取被选中的控件大小
	ScreenToClient(&rect_process);  //转化为客户区坐标

	if (point.x > rect_raw.left && point.x < rect_raw.right &&
		point.y > rect_raw.top && point.y < rect_raw.bottom)
	{
		int is_zoom_pixel_raw = ((CButton*)GetDlgItem(IDC_CHECK_PIXEL_RAW))->GetCheck();
		if (is_zoom_pixel_raw == BST_CHECKED)
		{
			int _x = (point.x - rect_raw.left - m_pos_raw_x_zero) / m_zoom_val_raw;
			int _y = (point.y - rect_raw.top - m_pos_raw_y_zero) / m_zoom_val_raw;
			m_pos_x = _x;
			m_pos_y = _y;

			CString x_str;
			x_str.Format(_T("X: %d"), _x);
			GetDlgItem(IDC_STATIC_POS_X)->SetWindowTextW(x_str);
			CString y_str;
			y_str.Format(_T("Y: %d"), _y);
			GetDlgItem(IDC_STATIC_POS_Y)->SetWindowTextW(y_str);
		}
		else
		{
			int _x = (point.x - rect_raw.left - m_pos_raw_x_zero);
			int _y = (point.y - rect_raw.top - m_pos_raw_y_zero);
			m_pos_x = _x;
			m_pos_y = _y;

			CString x_str;
			x_str.Format(_T("X: %d"), _x);
			GetDlgItem(IDC_STATIC_POS_X)->SetWindowTextW(x_str);
			CString y_str;
			y_str.Format(_T("Y: %d"), _y);
			GetDlgItem(IDC_STATIC_POS_Y)->SetWindowTextW(y_str);
		}
	}
	else if (point.x > rect_process.left && point.x < rect_process.right &&
		point.y > rect_process.top && point.y < rect_process.bottom)
	{
		int is_zoom_pixel_process = ((CButton*)GetDlgItem(IDC_CHECK_PIXEL_PROCESS))->GetCheck();
		if (is_zoom_pixel_process == BST_CHECKED)
		{
			int _x = (point.x - rect_process.left - m_pos_process_x_zero) / m_zoom_val_process;
			int _y = (point.y - rect_process.top - m_pos_process_y_zero) / m_zoom_val_process;
			m_pos_x = _x;
			m_pos_y = _y;

			CString x_str;
			x_str.Format(_T("X: %d"), _x);
			GetDlgItem(IDC_STATIC_POS_X)->SetWindowTextW(x_str);
			CString y_str;
			y_str.Format(_T("Y: %d"), _y);
			GetDlgItem(IDC_STATIC_POS_Y)->SetWindowTextW(y_str);
		}
		else
		{
			int _x = (point.x - rect_process.left - m_pos_process_x_zero);
			int _y = (point.y - rect_process.top - m_pos_process_y_zero);
			m_pos_x = _x;
			m_pos_y = _y;

			CString x_str;
			x_str.Format(_T("X: %d"), _x);
			GetDlgItem(IDC_STATIC_POS_X)->SetWindowTextW(x_str);
			CString y_str;
			y_str.Format(_T("Y: %d"), _y);
			GetDlgItem(IDC_STATIC_POS_Y)->SetWindowTextW(y_str);
		}
	}

	CDialogEx::OnMouseMove(nFlags, point);
}

void CImgProcess::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	if (nFlags == (MK_LBUTTON | MK_CONTROL))
	{
		CWnd* pWnd = GetFocus();
		int nId = pWnd->GetDlgCtrlID();
		CRect rect_raw;
		GetDlgItem(IDC_STATIC_PIC)->GetWindowRect(&rect_raw); //获取被选中的控件大小
		ScreenToClient(&rect_raw);  //转化为客户区坐标

		CRect rect_process;
		GetDlgItem(IDC_STATIC_PROCESS_PIC)->GetWindowRect(&rect_process); //获取被选中的控件大小
		ScreenToClient(&rect_process);  //转化为客户区坐标
		if ((point.x > rect_raw.left && point.x < rect_raw.right &&
			point.y > rect_raw.top && point.y < rect_raw.bottom) ||
			(point.x > rect_process.left && point.x < rect_process.right &&
				point.y > rect_process.top && point.y < rect_process.bottom))
		{
			switch (nId)
			{
			case IDC_X_1:
			case IDC_Y_1:
			{
				CString x_str;
				x_str.Format(_T("%d"), m_pos_x);
				SetDlgItemTextW(IDC_X_1, x_str);
				CString y_str;
				y_str.Format(_T("%d"), m_pos_y);
				SetDlgItemTextW(IDC_Y_1, y_str);
				break;
			}
			case IDC_X_2:
			case IDC_Y_2:
			{
				CString x_str;
				x_str.Format(_T("%d"), m_pos_x);
				SetDlgItemTextW(IDC_X_2, x_str);
				CString y_str;
				y_str.Format(_T("%d"), m_pos_y);
				SetDlgItemTextW(IDC_Y_2, y_str);
				break;
			}
			case IDC_X_3:
			case IDC_Y_3:
			{
				CString x_str;
				x_str.Format(_T("%d"), m_pos_x);
				SetDlgItemTextW(IDC_X_3, x_str);
				CString y_str;
				y_str.Format(_T("%d"), m_pos_y);
				SetDlgItemTextW(IDC_Y_3, y_str);
				break;
			}
			case IDC_X_4:
			case IDC_Y_4:
			{
				CString x_str;
				x_str.Format(_T("%d"), m_pos_x);
				SetDlgItemTextW(IDC_X_4, x_str);
				CString y_str;
				y_str.Format(_T("%d"), m_pos_y);
				SetDlgItemTextW(IDC_Y_4, y_str);
				break;
			}
			case IDC_X_U_1:
			case IDC_Y_U_1:
			{
				CString x_str;
				x_str.Format(_T("%d"), m_pos_x);
				SetDlgItemTextW(IDC_X_U_1, x_str);
				CString y_str;
				y_str.Format(_T("%d"), m_pos_y);
				SetDlgItemTextW(IDC_Y_U_1, y_str);
				break;
			}
			case IDC_X_U_2:
			case IDC_Y_U_2:
			{
				CString x_str;
				x_str.Format(_T("%d"), m_pos_x);
				SetDlgItemTextW(IDC_X_U_2, x_str);
				CString y_str;
				y_str.Format(_T("%d"), m_pos_y);
				SetDlgItemTextW(IDC_Y_U_2, y_str);
				break;
			}
			case IDC_X_U_3:
			case IDC_Y_U_3:
			{
				CString x_str;
				x_str.Format(_T("%d"), m_pos_x);
				SetDlgItemTextW(IDC_X_U_3, x_str);
				CString y_str;
				y_str.Format(_T("%d"), m_pos_y);
				SetDlgItemTextW(IDC_Y_U_3, y_str);
				break;
			}
			case IDC_X_U_4:
			case IDC_Y_U_4:
			{
				CString x_str;
				x_str.Format(_T("%d"), m_pos_x);
				SetDlgItemTextW(IDC_X_U_4, x_str);
				CString y_str;
				y_str.Format(_T("%d"), m_pos_y);
				SetDlgItemTextW(IDC_Y_U_4, y_str);
				break;
			}
			default:
				break;
			}
		}
	}

	CDialogEx::OnLButtonDown(nFlags, point);
}

BOOL CImgProcess::PreTranslateMessage(MSG* pMsg)
{
	switch (pMsg->message)
	{
	case WM_MOUSEMOVE:
	{
		m_toolTip.RelayEvent(pMsg);
	}
	default:
		break;
	}
	//if (pMsg->message == WM_MOUSEMOVE) {
	//	m_toolTip.RelayEvent(pMsg);
	//}
	return CDialogEx::PreTranslateMessage(pMsg);
}

// 二值化
unsigned char** CImgProcess::InternalBinaryProcess(unsigned char** pixel_mat_param, cv::Mat& img_process_param)
{
	try
	{
		CString str_value;
		GetDlgItem(IDC_EDIT_THRESHOLD_VALUE)->GetWindowText(str_value);
		unsigned char threshold_val = _ttoi(str_value);
		BinaryzationProcess(pixel_mat_param, img_process_param.rows, img_process_param.cols, threshold_val);
		// 更新阈值显示
		SetDlgItemTextW(IDC_EDIT_THRESHOLD_VALUE, CString(std::string(std::to_string(threshold_val)).c_str()));
		for (int i = 0; i < img_process_param.rows; ++i)
		{
			for (int j = 0; j < img_process_param.cols; ++j)
			{
				img_process_param.at<uchar>(i, j) = pixel_mat_param[i][j];
			}
		}
	}
	catch (...)
	{
		throw MyException("二值化dll模块出现未知错误. 若无法自行修复, 请联系QQ:842474373");
	}
	return pixel_mat_param;
}

unsigned char** CImgProcess::InternalAuxiliaryProcess(unsigned char** pixel_mat_param, cv::Mat& img_process_param)
{
	try
	{
		// 辅助线
		CString str_value;
		GetDlgItem(IDC_EDIT_THRESHOLD_VALUE)->GetWindowText(str_value);
		int threshold_val = _ttoi(str_value);

		left_line = new size_t[img_process_param.rows];
		mid_line = new size_t[img_process_param.rows];
		right_line = new size_t[img_process_param.rows];
		rofirger::add_log(rofirger::LOG_LEVEL_INFO, "new left_line, mid_line and right_line arrays size of %d bytes", img_process_param.rows);

		AuxiliaryProcess(pixel_mat_param, img_process_param.rows, img_process_param.cols, threshold_val, left_line, mid_line, right_line);
		for (int i = 0; i < img_process_param.rows; ++i)
		{
			for (int j = 0; j < img_process_param.cols; ++j)
			{
				img_process_param.at<uchar>(i, j) = pixel_mat_param[i][j];
			}
		}
	}
	catch (...)
	{
		throw MyException("辅助线dll模块出现未知错误. 若无法自行修复, 请联系QQ:842474373");
	}
	return pixel_mat_param;
}

unsigned char** CImgProcess::InternalPerspectiveProcess(unsigned char** pixel_mat_param, cv::Mat& img_process_param, int max_zoom_val_param)
{
	try
	{
		double** transform_mat = new double* [3];
		for (int i = 0; i < 3; ++i)
		{
			transform_mat[i] = new double[3];
		}
		for (int i = 0; i < 3; ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				transform_mat[i][j] = perspective_transform_mat.at<double>(i, j);
			}
		}
		// 缩放图片
		int zoom_val_perspective = m_slider_zoom_perspective.GetPos() < max_zoom_val_param ? m_slider_zoom_perspective.GetPos() : max_zoom_val_param;
		m_zoom_val_perspective = zoom_val_perspective;
		m_slider_zoom_perspective.SetPos(zoom_val_perspective);

		int old_rows = img_process_param.rows;
		int old_cols = img_process_param.cols;
		int dst_rows = old_rows * zoom_val_perspective;
		int dst_cols = old_cols * zoom_val_perspective;

		unsigned char** dst_img_mat = PerspectiveTransformProcess(pixel_mat_param, old_rows, old_cols, dst_rows, dst_cols, transform_mat);
		cv::resize(img_process_param, img_process_param, Size(dst_cols, dst_rows));
		/* 若存在辅助线，一并逆透视处理 */
		if (left_line != NULL && mid_line != NULL && right_line != NULL)
		{
			perspective_left_line.resize(old_rows);
			perspective_mid_line.resize(old_rows);
			perspective_right_line.resize(old_rows);
			for (size_t i_ = 0; i_ < old_rows; ++i_)
			{
				double x, y, w;
				x = transform_mat[0][0] * left_line[i_] + transform_mat[0][1] * i_ + transform_mat[0][2];
				y = transform_mat[1][0] * left_line[i_] + transform_mat[1][1] * i_ + transform_mat[1][2];
				w = transform_mat[2][0] * left_line[i_] + transform_mat[2][1] * i_ + transform_mat[2][2];
				int dst_col = (int)round(x / w);
				int dst_row = (int)round(y / w);
				if (dst_col >= 0 && dst_col < dst_cols
					&& dst_row >= 0 && dst_row < dst_rows)
				{
					perspective_left_line[i_].x = dst_col;
					perspective_left_line[i_].y = dst_row;
				}

				x = transform_mat[0][0] * mid_line[i_] + transform_mat[0][1] * i_ + transform_mat[0][2];
				y = transform_mat[1][0] * mid_line[i_] + transform_mat[1][1] * i_ + transform_mat[1][2];
				w = transform_mat[2][0] * mid_line[i_] + transform_mat[2][1] * i_ + transform_mat[2][2];
				dst_col = (int)round(x / w);
				dst_row = (int)round(y / w);
				if (dst_col >= 0 && dst_col < dst_cols
					&& dst_row >= 0 && dst_row < dst_rows)
				{
					perspective_mid_line[i_].x = dst_col;
					perspective_mid_line[i_].y = dst_row;
				}

				x = transform_mat[0][0] * right_line[i_] + transform_mat[0][1] * i_ + transform_mat[0][2];
				y = transform_mat[1][0] * right_line[i_] + transform_mat[1][1] * i_ + transform_mat[1][2];
				w = transform_mat[2][0] * right_line[i_] + transform_mat[2][1] * i_ + transform_mat[2][2];
				dst_col = (int)round(x / w);
				dst_row = (int)round(y / w);
				if (dst_col >= 0 && dst_col < dst_cols
					&& dst_row >= 0 && dst_row < dst_rows)
				{
					perspective_right_line[i_].x = dst_col;
					perspective_right_line[i_].y = dst_row;
				}
			}
		}

		for (int i = 0; i < dst_rows; ++i)
		{
			for (int j = 0; j < dst_cols; ++j)
			{
				img_process_param.at<uchar>(i, j) = dst_img_mat[i][j];
			}
		}
		// 改变 pixel_mat_param
		if (zoom_val_perspective != 1)
		{
			for (int i = 0; i < old_rows; ++i)
			{
				delete[]pixel_mat_param[i];
			}
			delete[]pixel_mat_param;
			pixel_mat_param = new unsigned char* [dst_rows];
			for (int i = 0; i < dst_rows; ++i)
			{
				pixel_mat_param[i] = new unsigned char[dst_cols];
			}
		}
		for (int i = 0; i < dst_rows; ++i)
		{
			for (int j = 0; j < dst_cols; ++j)
			{
				pixel_mat_param[i][j] = dst_img_mat[i][j];
			}
		}
		DeleteDstMat(dst_rows);
		for (int i = 0; i < 3; ++i)
		{
			delete[]transform_mat[i];
		}
		delete[]transform_mat;
	}
	catch (...)
	{
		throw MyException("透视变换dll模块出现未知错误. 若无法自行修复, 请联系QQ:842474373");
	}

	return pixel_mat_param;
}
UserProcessRet CImgProcess::InternalUserProcess(unsigned char** pixel_mat_param, cv::Mat& img_process_param, int max_zoom_val_param, bool& is_show_left_line, bool& is_show_mid_line, bool& is_show_right_line)
{
	try
	{
		// 为用户提供RGB绘点
		if (user_rgb_mat_vec.size() == img_process_param.rows && user_rgb_mat_vec[0].size() == img_process_param.cols)
		{
			for (int i = 0; i < img_process_param.rows; ++i)
			{
				for (int j = 0; j < img_process_param.cols; ++j)
				{
					user_rgb_mat_vec[i][j].r = 0;
					user_rgb_mat_vec[i][j].g = 0;
					user_rgb_mat_vec[i][j].b = 0;
					user_rgb_mat_vec[i][j].is_show = false;

				}
			}
		}
		else
		{
			for (int i = 0; i < user_rgb_mat_vec.size(); ++i)
			{
				user_rgb_mat_vec[i].clear();
			}
			user_rgb_mat_vec.clear();
			UserRGB temp;
			temp.r = 0; temp.g = 0, temp.b = 0; temp.is_show = false;
			std::vector<UserRGB> temp_vec;
			for (int i = 0; i < img_process_param.cols; ++i)
			{
				temp_vec.push_back(temp);
			}
			for (int i = 0; i < img_process_param.rows; ++i)
			{
				user_rgb_mat_vec.push_back(temp_vec);
			}

		}
		UserRGB** user_rgb_mat;
		user_rgb_mat = new UserRGB * [img_process_param.rows];
		for (int i = 0; i < img_process_param.rows; ++i)
		{
			user_rgb_mat[i] = new UserRGB[img_process_param.cols];
		}
		for (int i = 0; i < img_process_param.rows; ++i)
		{
			for (int j = 0; j < img_process_param.cols; ++j)
			{
				user_rgb_mat[i][j].r = 0;
				user_rgb_mat[i][j].g = 0;
				user_rgb_mat[i][j].b = 0;
				user_rgb_mat[i][j].is_show = false;
			}
		}

		CString str_value;
		GetDlgItem(IDC_EDIT_THRESHOLD_VALUE)->GetWindowText(str_value);
		unsigned int threshold_val = _ttoi(str_value);
		double** transform_mat = new double* [3];
		for (int i = 0; i < 3; ++i)
		{
			transform_mat[i] = new double[3];
		}
		for (int i = 0; i < 3; ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				transform_mat[i][j] = perspective_transform_mat.at<double>(i, j);
			}
		}
		int old_rows = img_process_param.rows;
		CString temp_user_input;
		GetDlgItemTextW(IDC_EDIT_USER_INPUT, temp_user_input);
		user_input.clear();
		int length_data = temp_user_input.GetLength();
		if (length_data != 0)
		{
			char* user_input_data = new  char[length_data + 1];
			user_input_data[0] = '\0';
			LPWSTR t_u_i = temp_user_input.GetBuffer();
			for (int i = 0; i < length_data; ++i)
			{
				user_input_data[i] = t_u_i[i];
			}
			temp_user_input.ReleaseBuffer();
			user_input_data[length_data] = '\0';
			user_input = user_input_data;
			delete[]user_input_data;
		}
		UserProcessRet ret = UserProcess(pixel_mat_param, img_process_param.rows, img_process_param.cols, user_rgb_mat,
			left_line, mid_line, right_line,
			is_show_left_line, is_show_mid_line, is_show_right_line,
			threshold_val, transform_mat, user_input, user_output, &slope);
		cv::resize(img_process_param, img_process_param, Size(ret.dst_cols, ret.dst_rows));
		// 修改阈值
		SetDlgItemTextW(IDC_EDIT_THRESHOLD_VALUE, CString(std::to_string(threshold_val).c_str()));

		// RGB服务
		for (int i = 0; i < img_process_param.rows; ++i)
		{
			for (int j = 0; j < img_process_param.cols; ++j)
			{
				user_rgb_mat_vec[i][j].r = user_rgb_mat[i][j].r;
				user_rgb_mat_vec[i][j].g = user_rgb_mat[i][j].g;
				user_rgb_mat_vec[i][j].b = user_rgb_mat[i][j].b;
				user_rgb_mat_vec[i][j].is_show = user_rgb_mat[i][j].is_show;
			}
		}
		for (int i = 0; i < img_process_param.rows; ++i)
		{
			delete[]user_rgb_mat[i];
		}
		delete[]user_rgb_mat;

		// 显示用户需要输出的文本
		SetDlgItemTextW(IDC_EDIT_USER_OUTPUT, CString(user_output.c_str()));
		m_user_output.LineScroll(m_user_output.GetLineCount() - 1, 0);
		for (int i = 0; i < ret.dst_rows; ++i)
		{
			for (int j = 0; j < ret.dst_cols; ++j)
			{
				img_process_param.at<uchar>(i, j) = ret.dst_pixel_mat[i][j];
			}
		}
		// 改变 pixel_mat_param
		for (int i = 0; i < old_rows; ++i)
		{
			delete[]pixel_mat_param[i];
		}
		delete[]pixel_mat_param;

		pixel_mat_param = new unsigned char* [ret.dst_rows];
		for (int i = 0; i < ret.dst_rows; ++i)
		{
			pixel_mat_param[i] = new unsigned char[ret.dst_cols];
		}
		for (int i = 0; i < ret.dst_rows; ++i)
		{
			for (int j = 0; j < ret.dst_cols; ++j)
			{
				pixel_mat_param[i][j] = ret.dst_pixel_mat[i][j];
			}
		}

		UserDeleteDstMat(ret.dst_pixel_mat, ret.dst_rows);
		ret.dst_pixel_mat = pixel_mat_param;
		for (int i = 0; i < 3; ++i)
		{
			delete[]transform_mat[i];
		}
		delete[]transform_mat;
		return ret;
	}
	catch (const char* msg)
	{
		std::string head("用户自定义:");
		head += msg;
		throw head.c_str();
	}
	catch (std::exception& e)
	{
		throw;
	}
	catch (...)
	{
		throw MyException("用户自定义dll模块出现未知错误. 若无法自行修复, 请联系QQ:842474373");
	}
}

void CImgProcess::OnNMClickList2(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	if (perspective_transform_mat.cols != 3 || perspective_transform_mat.rows != 3)
	{
		return;
	}
	// TODO: 在此添加控件通知处理程序代码
	CString source;
	source.Format(_T("%lf\t%lf\t%lf\r\n%lf\t%lf\t%lf\r\n%lf\t%lf\t%lf"),
		perspective_transform_mat.at<double>(0, 0), perspective_transform_mat.at<double>(0, 1), perspective_transform_mat.at<double>(0, 2),
		perspective_transform_mat.at<double>(1, 0), perspective_transform_mat.at<double>(1, 1), perspective_transform_mat.at<double>(1, 2),
		perspective_transform_mat.at<double>(2, 0), perspective_transform_mat.at<double>(2, 1), perspective_transform_mat.at<double>(2, 2));
	if (OpenClipboard())
	{
		HGLOBAL clipbuffer;
		char* buffer;
		EmptyClipboard();
		int len = wcslen(source);
		clipbuffer = GlobalAlloc(GMEM_DDESHARE, len + 1);
		buffer = (char*)GlobalLock(clipbuffer);
		LPWSTR src = source.GetBuffer();
		for (int i = 0; i <= len; ++i)
		{
			buffer[i] = src[i];
		}
		source.ReleaseBuffer();
		GlobalUnlock(clipbuffer);
		SetClipboardData(CF_TEXT, clipbuffer);
		CloseClipboard();
		::MessageBox(NULL, _T("已复制到剪切板"), _T("提示"), 0);
	}
	*pResult = 0;
}

void CImgProcess::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CSliderCtrl* pSlider = (CSliderCtrl*)pScrollBar;
	int pos = pSlider->GetPos();
	if (pScrollBar == (CScrollBar*)GetDlgItem(IDC_SLIDER1))
	{
		m_toolTip.Update();//更新tip
	}
	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CImgProcess::OnNMCustomdrawSlider2(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	str.Format(_T("%d"), m_slider_zoom_raw.GetPos());
	m_toolTip.UpdateTipText(str, GetDlgItem(IDC_SLIDER2));

	*pResult = 0;
}

void CImgProcess::OnNMCustomdrawSliderPerspective(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	str.Format(_T("%d"), m_slider_zoom_perspective.GetPos());
	m_toolTip.UpdateTipText(str, GetDlgItem(IDC_SLIDER3));
	m_zoom_val_perspective = m_slider_zoom_perspective.GetPos();
	*pResult = 0;
}

void CImgProcess::OnNMCustomdrawSlider1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	str.Format(_T("%d"), m_slider_zoom_process.GetPos());
	m_toolTip.UpdateTipText(str, GetDlgItem(IDC_SLIDER1));
	*pResult = 0;
}

void CImgProcess::OnBnClickedSaveRaw()
{
	// TODO: 在此添加控件通知处理程序代码
	int state = ((CButton*)GetDlgItem(IDC_SAVE_RAW))->GetCheck();
	if (state == BST_CHECKED)
	{
		CHAR            szFolderPath[255] = { 0 };
		CString          strFolderPath;
		BROWSEINFO       sInfo;
		::ZeroMemory(&sInfo, sizeof(BROWSEINFO));
		sInfo.pidlRoot = 0;
		sInfo.lpszTitle = _T("请选择缓存摄像头原图像文件所在文件夹");
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
		path_raw = szFolderPath;
		SetDlgItemTextW(IDC_RAW_PATH, strFolderPath);
	}
}

void CImgProcess::OnBnClickedSaveProcess()
{
	// TODO: 在此添加控件通知处理程序代码
	int state = ((CButton*)GetDlgItem(IDC_SAVE_PROCESS))->GetCheck();
	if (state == BST_CHECKED)
	{
		CHAR            szFolderPath[255] = { 0 };
		CString          strFolderPath;
		BROWSEINFO       sInfo;
		::ZeroMemory(&sInfo, sizeof(BROWSEINFO));
		sInfo.pidlRoot = 0;
		sInfo.lpszTitle = _T("请选择缓存已处理图像所在文件夹");
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
		path_process = szFolderPath;
		SetDlgItemTextW(IDC_PROCESS_PATH, strFolderPath);
	}
}

void CImgProcess::OnPaint()
{
	CPaintDC dc(this);
	// 原图像
	int pixelBytes = paint_img_raw.channels() * (paint_img_raw.depth() + 1); // 计算一个像素多少个字节
		// 制作bitmapinfo(数据头)
	BITMAPINFO bitInfo;
	bitInfo.bmiHeader.biBitCount = 8 * pixelBytes;
	bitInfo.bmiHeader.biWidth = paint_img_raw.cols;
	bitInfo.bmiHeader.biHeight = -paint_img_raw.rows;   //注意"-"号(正数时倒着绘制)
	bitInfo.bmiHeader.biPlanes = 1;
	bitInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bitInfo.bmiHeader.biCompression = BI_RGB;
	bitInfo.bmiHeader.biClrImportant = 0;
	bitInfo.bmiHeader.biClrUsed = 0;
	bitInfo.bmiHeader.biSizeImage = 0;
	bitInfo.bmiHeader.biXPelsPerMeter = 0;
	bitInfo.bmiHeader.biYPelsPerMeter = 0;
	BITMAPINFO_X pBmpInfo;
	memcpy(&pBmpInfo.bmiHeader, &bitInfo, sizeof(BITMAPINFOHEADER));
	//构造灰度图的调色版
	for (int i = 0; i < 256; i++)
	{
		pBmpInfo.bmiColors[i].rgbBlue = i;
		pBmpInfo.bmiColors[i].rgbGreen = i;
		pBmpInfo.bmiColors[i].rgbRed = i;
		pBmpInfo.bmiColors[i].rgbReserved = 0;
	}

	CRect rect;
	GetDlgItem(IDC_STATIC_PIC)->GetClientRect(&rect);  // 获取图片控件矩形框
	CDC* pDC = GetDlgItem(IDC_STATIC_PIC)->GetDC();  //获取图片控件DC
	int nWindowW = rect.Width();				                  //获取窗口宽度
	int nWindowH = rect.Height();				                  //获取窗口高度
	int nImageW = paint_img_raw.cols;				//获取图片宽度
	int nImageH = paint_img_raw.rows;				//获取图片高度
	// 更新图片原点坐标
	ScreenToClient(&rect);
	m_pos_raw_x_zero = (int)(rect.Width() - nImageW) / 2;
	m_pos_raw_y_zero = (int)(rect.Height() - nImageH) / 2;
	//绘图
	::StretchDIBits(
		pDC->GetSafeHdc(),
		(int)(nWindowW - nImageW) / 2, (int)(nWindowH - nImageH) / 2, paint_img_raw.cols, paint_img_raw.rows,
		0, 0, paint_img_raw.cols, paint_img_raw.rows,
		paint_img_raw.data,
		(BITMAPINFO*)&pBmpInfo,
		DIB_RGB_COLORS,
		SRCCOPY
	);
	ReleaseDC(pDC);  //释放DC

	// 处理后的图像
	CRect rect_;
	GetDlgItem(IDC_STATIC_PROCESS_PIC)->GetClientRect(&rect_);  // 获取图片控件矩形框
	int pixelBytes_ = paint_img_process.channels() * (paint_img_process.depth() + 1); // 计算一个像素多少个字节
	// 制作bitmapinfo(数据头)
	BITMAPINFO bitInfo_;
	bitInfo_.bmiHeader.biBitCount = 8 * pixelBytes_;
	bitInfo_.bmiHeader.biWidth = paint_img_process.cols;
	bitInfo_.bmiHeader.biHeight = -paint_img_process.rows;   //注意"-"号(正数时倒着绘制)
	bitInfo_.bmiHeader.biPlanes = 1;
	bitInfo_.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bitInfo_.bmiHeader.biCompression = BI_RGB;
	bitInfo_.bmiHeader.biClrImportant = 0;
	bitInfo_.bmiHeader.biClrUsed = 0;
	bitInfo_.bmiHeader.biSizeImage = 0;
	bitInfo_.bmiHeader.biXPelsPerMeter = 0;
	bitInfo_.bmiHeader.biYPelsPerMeter = 0;
	BITMAPINFO_X pBmpInfo_;
	memcpy(&pBmpInfo_.bmiHeader, &bitInfo_, sizeof(BITMAPINFOHEADER));
	//构造灰度图的调色版
	for (int i = 0; i < 256; i++)
	{
		pBmpInfo_.bmiColors[i].rgbBlue = i;
		pBmpInfo_.bmiColors[i].rgbGreen = i;
		pBmpInfo_.bmiColors[i].rgbRed = i;
		pBmpInfo_.bmiColors[i].rgbReserved = 0;
	}

	CDC* pDC_ = GetDlgItem(IDC_STATIC_PROCESS_PIC)->GetDC();  //获取图片控件DC
	int nWindowW_ = rect_.Width();				                  //获取窗口宽度
	int nWindowH_ = rect_.Height();				                  //获取窗口高度
	int nImageW_ = paint_img_process.cols;				//获取图片宽度
	int nImageH_ = paint_img_process.rows;				//获取图片高度
	ScreenToClient(&rect_);
	m_pos_process_x_zero = (int)(rect_.Width() - nImageW_) / 2;
	m_pos_process_y_zero = (int)(rect_.Height() - nImageH_) / 2;
	//绘图
	::StretchDIBits(
		pDC_->GetSafeHdc(),
		(int)(nWindowW_ - nImageW_) / 2, (int)(nWindowH_ - nImageH_) / 2, paint_img_process.cols, paint_img_process.rows,
		0, 0, paint_img_process.cols, paint_img_process.rows,
		paint_img_process.data,
		(BITMAPINFO*)&pBmpInfo_,
		DIB_RGB_COLORS,
		SRCCOPY
	);

	// RGB服务
	if (this->is_have_user_process)
	{
		for (int i = this->user_rgb_mat_vec.size() * m_zoom_val_process - 1; i >= 0; --i)
		{
			for (int j = this->user_rgb_mat_vec[0].size() * m_zoom_val_process - 1; j >= 0; --j)
			{
				if (this->user_rgb_mat_vec[i / m_zoom_val_process][j / m_zoom_val_process].is_show)
				{
					pDC_->SetPixel(this->m_pos_process_x_zero + j, this->m_pos_process_y_zero + i, RGB(this->user_rgb_mat_vec[i / m_zoom_val_process][j / m_zoom_val_process].r, this->user_rgb_mat_vec[i / m_zoom_val_process][j / m_zoom_val_process].g, this->user_rgb_mat_vec[i / m_zoom_val_process][j / m_zoom_val_process].b));
				}
			}
		}
	}
	ReleaseDC(pDC_);  //释放DC
					   // 不为绘图消息调用 CDialogEx::OnPaint()
}
BEGIN_MESSAGE_MAP(CDragListCtrl, CListCtrl)
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_NOTIFY_REFLECT(LVN_BEGINDRAG, &CDragListCtrl::OnLvnBegindrag)
END_MESSAGE_MAP()

void CDragListCtrl::OnLvnBegindrag(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;

	int count = GetSelectedCount();
	if (1 != count)
		return;

	POSITION pos = GetFirstSelectedItemPosition();
	if (NULL == pos)
		return;

	m_nSelItem = GetNextSelectedItem(pos);

	if (-1 == m_nSelItem)
		return;

	CPoint pt = pNMLV->ptAction;
	m_pDragImageList = CreateDragImage(m_nSelItem, &pt);
	if (NULL == m_pDragImageList)
		return;

	m_bDragging = TRUE;
	m_pDragImageList->BeginDrag(0, CPoint(8, 8));
	ClientToScreen(&pt);
	CRect rt;
	GetParent()->GetWindowRect(&rt);
	pt.x -= rt.left;
	pt.y -= rt.top;
	m_pDragImageList->DragEnter(GetParent(), pt);
	SetCapture();
}

void CDragListCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (m_bDragging)
	{
		CPoint pt = point;
		ClientToScreen(&pt);

		CRect rt;
		GetParent()->GetWindowRect(&rt);
		pt.x -= rt.left;
		pt.y -= rt.top;
		m_pDragImageList->DragMove(pt);
	}

	CListCtrl::OnMouseMove(nFlags, point);
}

void CDragListCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (m_bDragging)
	{
		ReleaseCapture();
		m_bDragging = FALSE;
		//m_nSelItem = -1;
		m_pDragImageList->DragLeave(GetParent());
		m_pDragImageList->EndDrag();
		m_pDragImageList->DeleteImageList();

		delete m_pDragImageList;
		m_pDragImageList = NULL;
	}

	CListCtrl::OnLButtonUp(nFlags, point);
}

void CImgProcess::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	CString img_("保存摄像头原图");
	CString text_;
	GetDlgItemTextW(IDC_SAVE_RAW, text_);
	if (img_ == text_)
	{
		SetDlgItemTextW(IDC_SAVE_RAW, _T("保存原像素矩阵"));
		SetDlgItemTextW(IDC_SAVE_PROCESS, _T("保存已像素矩阵"));
	}
	else
	{
		SetDlgItemTextW(IDC_SAVE_RAW, _T("保存摄像头原图"));
		SetDlgItemTextW(IDC_SAVE_PROCESS, _T("保存已处理图像"));
	}
}

void CImgProcess::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	CPixelSrc pixel_src_dlg(this);
	pixel_src_dlg.DoModal();
}

void CImgProcess::OnDropFiles(HDROP hDropInfo)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	int  nFileCount = ::DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 256);   //获取拖入的文件数量

	for (int i = 0; i < nFileCount; i++)
	{
		CString m_file;// 完整路径
		UINT nChars = ::DragQueryFile(hDropInfo, i, m_file.GetBuffer(256), 256);   // 获取拖入的第i个文件的文件名
		m_file.ReleaseBuffer();
		drop_files.push_back(m_file);
	}
	::DragFinish(hDropInfo);
	if (!is_close_look)
	{
		file_look->Reflesh();
	}
	else
	{
		if (file_look != NULL)
		{
			delete file_look;
		}
		file_look = new CFileLook(drop_files, is_close_look);
		file_look->Create(IDD_FILE_LOOK);
		file_look->ShowWindow(SW_SHOWNORMAL);
	}
	CDialogEx::OnDropFiles(hDropInfo);
}

void CImgProcess::OnBnClickedButton7()
{
	// TODO: 在此添加控件通知处理程序代码
	CString now_botton_text;
	GetDlgItemTextW(IDC_BUTTON7, now_botton_text);
	if (is_pixel_src_checked && now_botton_text == CString("开始"))
	{
		if (pixel_src_file_path_selected.size() == 0)
		{
			::MessageBox(NULL, _T("无像素源. 请添加像素文件"), _T("提示"), 0);
			return;
		}
		is_start_pixel_src = true;
		SetDlgItemText(IDC_BUTTON7, _T("暂停"));

		WriteOperationInfoToFile();

		// 求取透视变换矩阵
		GetPerspectiveTransformMat();

		// 更新list_control
		CString str_temp1, str_temp2, str_temp3;
		for (int i = 0; i < 3; ++i)
		{
			str_temp1.Format(_T("%lf"), perspective_transform_mat.at<double>(i, 0));
			str_temp2.Format(_T("%lf"), perspective_transform_mat.at<double>(i, 1));
			str_temp3.Format(_T("%lf"), perspective_transform_mat.at<double>(i, 2));
			m_list_transform.SetItemText(i, 0, str_temp1);
			m_list_transform.SetItemText(i, 1, str_temp2);
			m_list_transform.SetItemText(i, 2, str_temp3);
		}
		EnableControlForPixelSrc(this, false);
		GetPixelPicInfo();
		refresh_timer.StartTimer(interval, std::bind(RefleshPicPixel, this));
	}
	else if (now_botton_text != CString("开始"))
	{
		SetDlgItemText(IDC_BUTTON7, _T("开始"));
		is_start_pixel_src = false;
		refresh_timer.Expire();
		EnableControlForPixelSrc(this, true);
		// 更新 IDC_PIXEL_SRC_PATH 显示
		SetDlgItemTextW(IDC_PIXEL_SRC_PATH, _T(""));
	}
}
void CImgProcess::GetPixelPicInfo()
{
	for (int i = 0; i < pixel_src_file_path_selected.size(); ++i)
	{
		PICINFO temp_info;
		pic_info.push_back(temp_info);
		std::fstream out_file1;
		out_file1.open((pixel_src_path + pixel_src_file_path_selected[i]).GetBuffer(), std::ios::in | std::ios::out | std::ios::binary);
		out_file1.seekg(-(int)sizeof(int), std::ios::end);
		out_file1.read((char*)&pic_info[i].pic_kind, sizeof(pic_info[i].pic_kind));
		out_file1.seekg(-(int)sizeof(int) * 2, std::ios::end);
		out_file1.read((char*)&pic_info[i].pic_height, sizeof(pic_info[i].pic_height));
		out_file1.seekg(-(int)sizeof(int) * 3, std::ios::end);
		out_file1.read((char*)&pic_info[i].pic_width, sizeof(pic_info[i].pic_width));
		out_file1.close();
		(pixel_src_path + pixel_src_file_path_selected[i]).ReleaseBuffer();
		if (pic_info[i].pic_kind < 0 || pic_info[i].pic_height <= 0 || pic_info[i].pic_width <= 0)
		{
			::MessageBox(NULL, _T("像素文件:") + (pixel_src_path + pixel_src_file_path_selected[i]) + _T("有误!"), _T("警告"), 0);
			pic_info[i].pic_kind = 0;
			pic_info[i].pic_height = 1;
			pic_info[i].pic_width = 1;
		}
	}
}

void CImgProcess::OnBnClickedButton8()
{
	pixel_src_file_path_selected_index = 0;
}

void CImgProcess::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	// 配置
	CRect client_rect;
	//CRect rect_static_border;
	GetClientRect(&client_rect);
	CRect rect_tab_mode;
	CWnd* pWnd_rect_tab_mode = GetDlgItem(IDC_TAB_MODE);
	if (pWnd_rect_tab_mode)
	{
		pWnd_rect_tab_mode->GetWindowRect(&rect_tab_mode);
		ScreenToClient(&rect_tab_mode);
	}

	/*CWnd* pWnd_static_border = GetDlgItem(IDC_STATIC_BORDER);

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
	}*/

	// 左下角区域
	CWnd* pWnd_static_binary = GetDlgItem(IDC_STATIC_BINARY);
	CWnd* pWnd_pictype = GetDlgItem(IDC_COMBO6);
	CWnd* pWnd_s_width = GetDlgItem(IDC_STATIC_WIDTH);
	CWnd* pWnd_edit_width = GetDlgItem(IDC_EDIT_WIDTH);
	CWnd* pWnd_s_height = GetDlgItem(IDC_STATIC_HEIGHT);
	CWnd* pWnd_edit_height = GetDlgItem(IDC_EDIT_HEIGTH);
	CWnd* pWnd_s_threshold = GetDlgItem(IDC_STATIC_THRESHOLD);
	CWnd* pWnd_edit_threshold = GetDlgItem(IDC_EDIT_THRESHOLD_VALUE);
	CWnd* pWnd_spin_threshold = GetDlgItem(IDC_SPIN2);

	CWnd* pWnd_static_perspective = GetDlgItem(IDC_STATIC_PERSPECTIVE);
	CWnd* pWnd_static_old = GetDlgItem(IDC_STATIC_OLD);
	CWnd* pWnd_x_1 = GetDlgItem(IDC_X_1);
	CWnd* pWnd_y_1 = GetDlgItem(IDC_Y_1);
	CWnd* pWnd_x_2 = GetDlgItem(IDC_X_2);
	CWnd* pWnd_y_2 = GetDlgItem(IDC_Y_2);
	CWnd* pWnd_x_3 = GetDlgItem(IDC_X_3);
	CWnd* pWnd_y_3 = GetDlgItem(IDC_Y_3);
	CWnd* pWnd_x_4 = GetDlgItem(IDC_X_4);
	CWnd* pWnd_y_4 = GetDlgItem(IDC_Y_4);
	CWnd* pWnd_static_new = GetDlgItem(IDC_STATIC_NEW);
	CWnd* pWnd_x_11 = GetDlgItem(IDC_X_U_1);
	CWnd* pWnd_y_11 = GetDlgItem(IDC_Y_U_1);
	CWnd* pWnd_x_22 = GetDlgItem(IDC_X_U_2);
	CWnd* pWnd_y_22 = GetDlgItem(IDC_Y_U_2);
	CWnd* pWnd_x_33 = GetDlgItem(IDC_X_U_3);
	CWnd* pWnd_y_33 = GetDlgItem(IDC_Y_U_3);
	CWnd* pWnd_x_44 = GetDlgItem(IDC_X_U_4);
	CWnd* pWnd_y_44 = GetDlgItem(IDC_Y_U_4);

	CWnd* pWnd_static_picsave = GetDlgItem(IDC_STATIC_SAVE_PIC);
	CWnd* pWnd_button_trans = GetDlgItem(IDC_BUTTON1);
	CWnd* pWnd_check_save_old = GetDlgItem(IDC_SAVE_RAW);
	CWnd* pWnd_check_save_new = GetDlgItem(IDC_SAVE_PROCESS);
	CWnd* pWnd_edit_save_old_path = GetDlgItem(IDC_RAW_PATH);
	CWnd* pWnd_edit_save_new_path = GetDlgItem(IDC_PROCESS_PATH);

	CWnd* pWnd_edit_input = GetDlgItem(IDC_EDIT_USER_INPUT);
	CWnd* pWnd_edit_ouput = GetDlgItem(IDC_EDIT_USER_OUTPUT);

	// 中部
	CWnd* pWnd_button_src = GetDlgItem(IDC_BUTTON2);
	CWnd* pWnd_button_start = GetDlgItem(IDC_BUTTON7);
	CWnd* pWnd_button_res = GetDlgItem(IDC_BUTTON8);
	CWnd* pWnd_button_single_step_left = GetDlgItem(IDC_BUTTON_SINGLE_STEP_LEFT);
	CWnd* pWnd_button_single_step_right = GetDlgItem(IDC_BUTTON_SINGLE_STEP_RIGHT);

	// 右部上
	CWnd* pWnd_edit_show_path = GetDlgItem(IDC_PIXEL_SRC_PATH);
	CWnd* pWnd_s_x = GetDlgItem(IDC_STATIC_POS_X);
	CWnd* pWnd_s_y = GetDlgItem(IDC_STATIC_POS_Y);
	CWnd* pWnd_s_tip = GetDlgItem(IDC_STATIC_TIP);
	CWnd* pWnd_static_show_oldpic = GetDlgItem(IDC_STATIC_SHOW_PIC);
	CWnd* pWnd_pic_old = GetDlgItem(IDC_STATIC_PIC);
	CWnd* pWnd_slider_old = GetDlgItem(IDC_SLIDER2);
	CWnd* pWnd_check_zoom_old = GetDlgItem(IDC_CHECK_PIXEL_RAW);

	// 右部中
	CWnd* pWnd_static_receive_pics = GetDlgItem(IDC_STATIC_RECEIVE_PICS);
	CWnd* pWnd_static_process = GetDlgItem(IDC_STATIC_PROCESS);
	CWnd* pWnd_combo_process_1 = GetDlgItem(IDC_COMBO_ONE);
	CWnd* pWnd_combo_process_2 = GetDlgItem(IDC_COMBO_TWO);
	CWnd* pWnd_combo_process_3 = GetDlgItem(IDC_COMBO_THREE);
	CWnd* pWnd_combo_process_4 = GetDlgItem(IDC_COMBO_FOUR);
	CWnd* pWnd_static_zoom_perspective = GetDlgItem(IDC_STATIC_ZOOM_PER);
	CWnd* pWnd_slider_zoom_perspective = GetDlgItem(IDC_SLIDER3);
	CWnd* pWnd_list_pertrans = GetDlgItem(IDC_LIST2);

	// 右部下
	CWnd* pWnd_button_veer = GetDlgItem(IDC_BUTTON_VEER);
	CWnd* pWnd_static_show_newpic = GetDlgItem(IDC_STATIC_SHOW_NEW);
	CWnd* pWnd_pic_new = GetDlgItem(IDC_STATIC_PROCESS_PIC);
	CWnd* pWnd_slider_new = GetDlgItem(IDC_SLIDER1);
	CWnd* pWnd_check_zoom_new = GetDlgItem(IDC_CHECK_PIXEL_PROCESS);
	if (pWnd_check_zoom_new)
	{
		// 左下角区域
		pWnd_static_binary->GetWindowRect(&rect_static_binary);
		ScreenToClient(&rect_static_binary);
		pWnd_pictype->GetWindowRect(&rect_pictype);
		ScreenToClient(&rect_pictype);
		pWnd_s_width->GetWindowRect(&rect_s_width);
		ScreenToClient(&rect_s_width);
		pWnd_edit_width->GetWindowRect(&rect_edit_width);
		ScreenToClient(&rect_edit_width);
		pWnd_s_height->GetWindowRect(&rect_s_height);
		ScreenToClient(&rect_s_height);
		pWnd_edit_height->GetWindowRect(&rect_edit_height);
		ScreenToClient(&rect_edit_height);
		pWnd_s_threshold->GetWindowRect(&rect_s_threshold);
		ScreenToClient(&rect_s_threshold);
		pWnd_edit_threshold->GetWindowRect(&rect_edit_threshold);
		ScreenToClient(&rect_edit_threshold);
		pWnd_spin_threshold->GetWindowRect(&rect_spin_threshold);
		ScreenToClient(&rect_spin_threshold);

		pWnd_static_perspective->GetWindowRect(&rect_static_perspective);
		ScreenToClient(&rect_static_perspective);
		pWnd_static_old->GetWindowRect(&rect_static_old);
		ScreenToClient(&rect_static_old);
		pWnd_x_1->GetWindowRect(&rect_x_1);
		ScreenToClient(&rect_x_1);
		pWnd_y_1->GetWindowRect(&rect_y_1);
		ScreenToClient(&rect_y_1);
		pWnd_x_2->GetWindowRect(&rect_x_2);
		ScreenToClient(&rect_x_2);
		pWnd_y_2->GetWindowRect(&rect_y_2);
		ScreenToClient(&rect_y_2);
		pWnd_x_3->GetWindowRect(&rect_x_3);
		ScreenToClient(&rect_x_3);
		pWnd_y_3->GetWindowRect(&rect_y_3);
		ScreenToClient(&rect_y_3);
		pWnd_x_4->GetWindowRect(&rect_x_4);
		ScreenToClient(&rect_x_4);
		pWnd_y_4->GetWindowRect(&rect_y_4);
		ScreenToClient(&rect_y_4);
		pWnd_static_new->GetWindowRect(&rect_static_new);
		ScreenToClient(&rect_static_new);
		pWnd_x_11->GetWindowRect(&rect_x_11);
		ScreenToClient(&rect_x_11);
		pWnd_y_11->GetWindowRect(&rect_y_11);
		ScreenToClient(&rect_y_11);
		pWnd_x_22->GetWindowRect(&rect_x_22);
		ScreenToClient(&rect_x_22);
		pWnd_y_22->GetWindowRect(&rect_y_22);
		ScreenToClient(&rect_y_22);
		pWnd_x_33->GetWindowRect(&rect_x_33);
		ScreenToClient(&rect_x_33);
		pWnd_y_33->GetWindowRect(&rect_y_33);
		ScreenToClient(&rect_y_33);
		pWnd_x_44->GetWindowRect(&rect_x_44);
		ScreenToClient(&rect_x_44);
		pWnd_y_44->GetWindowRect(&rect_y_44);
		ScreenToClient(&rect_y_44);

		pWnd_static_picsave->GetWindowRect(&rect_static_picsave);
		ScreenToClient(&rect_static_picsave);
		pWnd_button_trans->GetWindowRect(&rect_button_trans);
		ScreenToClient(&rect_button_trans);
		pWnd_check_save_old->GetWindowRect(&rect_check_save_old);
		ScreenToClient(&rect_check_save_old);
		pWnd_check_save_new->GetWindowRect(&rect_check_save_new);
		ScreenToClient(&rect_check_save_new);
		pWnd_edit_save_old_path->GetWindowRect(&rect_edit_save_old_path);
		ScreenToClient(&rect_edit_save_old_path);
		pWnd_edit_save_new_path->GetWindowRect(&rect_edit_save_new_path);
		ScreenToClient(&rect_edit_save_new_path);

		pWnd_edit_input->GetWindowRect(&rect_edit_input);
		ScreenToClient(&rect_edit_input);
		pWnd_edit_ouput->GetWindowRect(&rect_edit_ouput);
		ScreenToClient(&rect_edit_ouput);

		// 中部
		pWnd_button_src->GetWindowRect(&rect_button_src);
		ScreenToClient(&rect_button_src);
		pWnd_button_start->GetWindowRect(&rect_button_start);
		ScreenToClient(&rect_button_start);
		pWnd_button_res->GetWindowRect(&rect_button_res);
		ScreenToClient(&rect_button_res);
		pWnd_button_single_step_left->GetWindowRect(&rect_button_single_step_left);
		ScreenToClient(&rect_button_single_step_left);
		pWnd_button_single_step_right->GetWindowRect(&rect_button_single_step_right);
		ScreenToClient(&rect_button_single_step_right);

		// 右部上
		pWnd_edit_show_path->GetWindowRect(&rect_edit_show_path);
		ScreenToClient(&rect_edit_show_path);
		pWnd_s_x->GetWindowRect(&rect_s_x);
		ScreenToClient(&rect_s_x);
		pWnd_s_y->GetWindowRect(&rect_s_y);
		ScreenToClient(&rect_s_y);
		pWnd_s_tip->GetWindowRect(&rect_s_tip);
		ScreenToClient(&rect_s_tip);
		pWnd_static_show_oldpic->GetWindowRect(&rect_static_show_oldpic);
		ScreenToClient(&rect_static_show_oldpic);
		pWnd_pic_old->GetWindowRect(&rect_pic_old);
		ScreenToClient(&rect_pic_old);
		pWnd_slider_old->GetWindowRect(&rect_slider_old);
		ScreenToClient(&rect_slider_old);
		pWnd_check_zoom_old->GetWindowRect(&rect_check_zoom_old);
		ScreenToClient(&rect_check_zoom_old);

		// 右部中
		pWnd_static_receive_pics->GetWindowRect(&rect_static_receive_pics);
		pWnd_static_process->GetWindowRect(&rect_static_process);
		ScreenToClient(&rect_static_process);
		pWnd_combo_process_1->GetWindowRect(&rect_combo_process_1);
		ScreenToClient(&rect_combo_process_1);
		pWnd_combo_process_2->GetWindowRect(&rect_combo_process_2);
		ScreenToClient(&rect_combo_process_2);
		pWnd_combo_process_3->GetWindowRect(&rect_combo_process_3);
		ScreenToClient(&rect_combo_process_3);
		pWnd_combo_process_4->GetWindowRect(&rect_combo_process_4);
		ScreenToClient(&rect_combo_process_4);
		pWnd_static_zoom_perspective->GetWindowRect(&rect_static_zoom_perspective);
		ScreenToClient(&rect_static_zoom_perspective);
		pWnd_slider_zoom_perspective->GetWindowRect(&rect_slider_zoom_perspective);
		ScreenToClient(&rect_slider_zoom_perspective);
		pWnd_list_pertrans->GetWindowRect(&rect_list_pertrans);
		ScreenToClient(&rect_list_pertrans);

		// 右部下
		pWnd_button_veer->GetWindowRect(&rect_button_veer);
		ScreenToClient(&rect_button_veer);
		pWnd_static_show_newpic->GetWindowRect(&rect_static_show_newpic);
		ScreenToClient(&rect_static_show_newpic);
		pWnd_pic_new->GetWindowRect(&rect_pic_new);
		ScreenToClient(&rect_pic_new);
		pWnd_slider_new->GetWindowRect(&rect_slider_new);
		ScreenToClient(&rect_slider_new);
		pWnd_check_zoom_new->GetWindowRect(&rect_check_zoom_new);
		ScreenToClient(&rect_check_zoom_new);

		// 改变位置
		pWnd_static_binary->SetWindowPos(NULL, client_rect.left + 20, rect_tab_mode.bottom + 30,
			rect_s_threshold.Width() + rect_edit_width.Width() + 70, rect_edit_width.Height() * 3 + rect_pictype.Height() + 70,
			SWP_NOZORDER);
		pWnd_static_binary->GetWindowRect(&rect_static_binary);
		ScreenToClient(&rect_static_binary);
		pWnd_pictype->SetWindowPos(NULL, rect_static_binary.left + 15, rect_static_binary.top + 30, rect_static_binary.Width() - 30, rect_pictype.Height(), SWP_NOZORDER);
		pWnd_pictype->GetWindowRect(&rect_pictype);
		ScreenToClient(&rect_pictype);
		pWnd_s_width->SetWindowPos(NULL, rect_static_binary.left + 30, rect_pictype.bottom + 25, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
		pWnd_s_width->GetWindowRect(&rect_s_width);
		ScreenToClient(&rect_s_width);
		pWnd_edit_width->SetWindowPos(NULL, rect_pictype.right - 20 - rect_edit_width.Width(), rect_s_width.top - (rect_edit_width.Height() - rect_s_width.Height()) / 2, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		pWnd_edit_width->GetWindowRect(&rect_edit_width);
		ScreenToClient(&rect_edit_width);
		pWnd_s_height->SetWindowPos(NULL, rect_static_binary.left + 30, rect_s_width.bottom + 15, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		pWnd_s_height->GetWindowRect(&rect_s_height);
		ScreenToClient(&rect_s_height);
		pWnd_edit_height->SetWindowPos(NULL, rect_pictype.right - 20 - rect_edit_height.Width(), rect_s_height.top - (rect_edit_height.Height() - rect_s_height.Height()) / 2, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		pWnd_edit_height->GetWindowRect(&rect_edit_height);
		ScreenToClient(&rect_edit_height);
		pWnd_s_threshold->SetWindowPos(NULL, rect_s_width.left - (rect_s_threshold.Width() - rect_s_width.Width()) / 2, rect_s_height.bottom + 15, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		pWnd_s_threshold->GetWindowRect(&rect_s_threshold);
		ScreenToClient(&rect_s_threshold);
		pWnd_edit_threshold->SetWindowPos(NULL, rect_edit_width.left, rect_s_threshold.top - (rect_edit_threshold.Height() - rect_s_threshold.Height()) / 2, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		pWnd_edit_threshold->GetWindowRect(&rect_edit_threshold);
		ScreenToClient(&rect_edit_threshold);
		pWnd_spin_threshold->SetWindowPos(NULL, rect_edit_threshold.right, rect_edit_threshold.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		pWnd_spin_threshold->GetWindowRect(&rect_spin_threshold);
		ScreenToClient(&rect_spin_threshold);

		pWnd_static_perspective->SetWindowPos(NULL, rect_static_binary.right + 5, rect_static_binary.top, rect_x_1.Width() * 4 + 40, rect_x_1.Height() * 4 + 75, SWP_NOZORDER);
		pWnd_static_perspective->GetWindowRect(&rect_static_perspective);
		ScreenToClient(&rect_static_perspective);
		pWnd_static_old->SetWindowPos(NULL, rect_static_perspective.left + 5, rect_static_perspective.top + 20, rect_static_perspective.Width() - 10, rect_x_1.Height() * 2 + 5 + 23 + 5, SWP_NOZORDER);
		pWnd_static_old->GetWindowRect(&rect_static_old);
		ScreenToClient(&rect_static_old);
		pWnd_x_1->SetWindowPos(NULL, rect_static_old.left + 6, rect_static_old.top + 23, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		pWnd_x_1->GetWindowRect(&rect_x_1);
		ScreenToClient(&rect_x_1);
		pWnd_y_1->SetWindowPos(NULL, rect_x_1.right, rect_x_1.top, rect_x_1.Width(), rect_x_1.Height(), SWP_NOZORDER);
		pWnd_y_1->GetWindowRect(&rect_y_1);
		ScreenToClient(&rect_y_1);
		pWnd_x_2->SetWindowPos(NULL, rect_static_old.right - rect_x_1.Width() * 2 - 5, rect_x_1.top, rect_x_1.Width(), rect_x_1.Height(), SWP_NOZORDER);
		pWnd_x_2->GetWindowRect(&rect_x_2);
		ScreenToClient(&rect_x_2);
		pWnd_y_2->SetWindowPos(NULL, rect_static_old.right - rect_x_1.Width() - 5, rect_x_1.top, rect_x_1.Width(), rect_x_1.Height(), SWP_NOZORDER);
		pWnd_y_2->GetWindowRect(&rect_y_2);
		ScreenToClient(&rect_y_2);
		pWnd_x_3->SetWindowPos(NULL, rect_static_old.left + 6, rect_x_1.bottom + 5, rect_x_1.Width(), rect_x_1.Height(), SWP_NOZORDER);
		pWnd_x_3->GetWindowRect(&rect_x_3);
		ScreenToClient(&rect_x_3);
		pWnd_y_3->SetWindowPos(NULL, rect_x_3.right, rect_x_3.top, rect_x_1.Width(), rect_x_1.Height(), SWP_NOZORDER);
		pWnd_y_3->GetWindowRect(&rect_y_3);
		ScreenToClient(&rect_y_3);
		pWnd_x_4->SetWindowPos(NULL, rect_static_old.right - rect_x_1.Width() * 2 - 5, rect_x_3.top, rect_x_1.Width(), rect_x_1.Height(), SWP_NOZORDER);
		pWnd_x_4->GetWindowRect(&rect_x_4);
		ScreenToClient(&rect_x_4);
		pWnd_y_4->SetWindowPos(NULL, rect_static_old.right - rect_x_1.Width() - 5, rect_x_3.top, rect_x_1.Width(), rect_x_1.Height(), SWP_NOZORDER);
		pWnd_y_4->GetWindowRect(&rect_y_4);
		ScreenToClient(&rect_y_4);
		pWnd_static_new->SetWindowPos(NULL, rect_static_perspective.left + 5, rect_static_old.bottom, rect_static_old.Width(), rect_static_old.Height(), SWP_NOZORDER);
		pWnd_static_new->GetWindowRect(&rect_static_new);
		ScreenToClient(&rect_static_new);
		// 重新调整透视变换设置外围框
		pWnd_static_perspective->SetWindowPos(NULL, rect_static_binary.right + 5, rect_static_binary.top, rect_x_1.Width() * 4 + 40, rect_static_new.bottom - rect_static_binary.top + 5, SWP_NOZORDER);
		pWnd_static_perspective->GetWindowRect(&rect_static_perspective);
		ScreenToClient(&rect_static_perspective);
		pWnd_x_11->SetWindowPos(NULL, rect_static_new.left + 6, rect_static_new.top + 23, rect_x_1.Width(), rect_x_1.Height(), SWP_NOZORDER);
		pWnd_x_11->GetWindowRect(&rect_x_11);
		ScreenToClient(&rect_x_11);
		pWnd_y_11->SetWindowPos(NULL, rect_x_11.right, rect_x_11.top, rect_x_1.Width(), rect_x_1.Height(), SWP_NOZORDER);
		pWnd_y_11->GetWindowRect(&rect_y_11);
		ScreenToClient(&rect_y_11);
		pWnd_x_22->SetWindowPos(NULL, rect_static_new.right - rect_x_1.Width() * 2 - 5, rect_x_11.top, rect_x_1.Width(), rect_x_1.Height(), SWP_NOZORDER);
		pWnd_x_22->GetWindowRect(&rect_x_22);
		ScreenToClient(&rect_x_22);
		pWnd_y_22->SetWindowPos(NULL, rect_static_new.right - rect_x_1.Width() - 5, rect_x_11.top, rect_x_1.Width(), rect_x_1.Height(), SWP_NOZORDER);
		pWnd_y_22->GetWindowRect(&rect_y_22);
		ScreenToClient(&rect_y_22);
		pWnd_x_33->SetWindowPos(NULL, rect_static_new.left + 6, rect_x_11.bottom + 5, rect_x_1.Width(), rect_x_1.Height(), SWP_NOZORDER);
		pWnd_x_33->GetWindowRect(&rect_x_33);
		ScreenToClient(&rect_x_33);
		pWnd_y_33->SetWindowPos(NULL, rect_x_33.right, rect_x_33.top, rect_x_1.Width(), rect_x_1.Height(), SWP_NOZORDER);
		pWnd_y_33->GetWindowRect(&rect_y_33);
		ScreenToClient(&rect_y_33);
		pWnd_x_44->SetWindowPos(NULL, rect_static_new.right - rect_x_1.Width() * 2 - 5, rect_x_33.top, rect_x_1.Width(), rect_x_1.Height(), SWP_NOZORDER);
		pWnd_x_44->GetWindowRect(&rect_x_44);
		ScreenToClient(&rect_x_44);
		pWnd_y_44->SetWindowPos(NULL, rect_static_new.right - rect_x_1.Width() - 5, rect_x_33.top, rect_x_1.Width(), rect_x_1.Height(), SWP_NOZORDER);
		pWnd_y_44->GetWindowRect(&rect_y_44);
		ScreenToClient(&rect_y_44);

		pWnd_button_trans->SetWindowPos(NULL, rect_static_binary.left, rect_static_new.bottom + 20, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		pWnd_button_trans->GetWindowRect(&rect_button_trans);
		ScreenToClient(&rect_button_trans);
		pWnd_static_picsave->SetWindowPos(NULL, rect_button_trans.right, rect_button_trans.top - 10, rect_check_save_old.Width() + rect_edit_save_old_path.Width() + 15, rect_edit_save_old_path.Height() * 2 + 25 + 16, SWP_NOZORDER);
		pWnd_static_picsave->GetWindowRect(&rect_static_picsave);
		ScreenToClient(&rect_static_picsave);
		pWnd_button_trans->SetWindowPos(NULL, 0, 0, rect_button_trans.Width(), rect_static_picsave.Height() - 10, SWP_NOMOVE | SWP_NOZORDER);
		pWnd_button_trans->GetWindowRect(&rect_button_trans);
		ScreenToClient(&rect_button_trans);
		pWnd_edit_save_old_path->SetWindowPos(NULL, rect_static_picsave.right - 5 - rect_edit_save_old_path.Width(), rect_static_picsave.top + 25, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
		pWnd_edit_save_old_path->GetWindowRect(&rect_edit_save_old_path);
		ScreenToClient(&rect_edit_save_old_path);
		ScreenToClient(&rect_edit_save_old_path);
		pWnd_check_save_old->SetWindowPos(NULL, rect_static_picsave.left + 5, rect_static_picsave.top + 25 + (rect_edit_save_old_path.Height() - rect_check_save_old.Height()) / 2, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
		pWnd_check_save_old->GetWindowRect(&rect_check_save_old);
		ScreenToClient(&rect_check_save_old);
		pWnd_edit_save_new_path->SetWindowPos(NULL, rect_static_picsave.right - 5 - rect_edit_save_new_path.Width(), rect_static_picsave.bottom - rect_edit_save_new_path.Height() - 5, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		pWnd_edit_save_new_path->GetWindowRect(&rect_edit_save_new_path);
		ScreenToClient(&rect_edit_save_new_path);
		pWnd_check_save_new->SetWindowPos(NULL, rect_static_picsave.left + 5, rect_edit_save_new_path.top + (rect_edit_save_new_path.Height() - rect_check_save_new.Height()) / 2, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
		pWnd_check_save_new->GetWindowRect(&rect_check_save_new);
		ScreenToClient(&rect_check_save_new);

		pWnd_edit_input->SetWindowPos(NULL, rect_button_trans.left, rect_static_picsave.bottom + 15, rect_static_picsave.Width() + rect_button_trans.Width(), (client_rect.bottom - rect_static_picsave.bottom - 15 - 10 - 30) / 2, SWP_NOZORDER);
		pWnd_edit_input->GetWindowRect(&rect_edit_input);
		ScreenToClient(&rect_edit_input);
		pWnd_edit_ouput->SetWindowPos(NULL, rect_button_trans.left, rect_edit_input.bottom + 10, rect_edit_input.Width(), rect_edit_input.Height(), SWP_NOZORDER);
		pWnd_edit_ouput->GetWindowRect(&rect_edit_ouput);
		ScreenToClient(&rect_edit_ouput);

		// 中部
		pWnd_button_src->SetWindowPos(NULL, rect_tab_mode.right + 20, rect_tab_mode.top + 40, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		pWnd_button_src->GetWindowRect(&rect_button_src);
		ScreenToClient(&rect_button_src);
		pWnd_button_res->SetWindowPos(NULL, rect_tab_mode.right + 20, rect_tab_mode.bottom - rect_button_src.Height() - 40, rect_button_src.Width(), rect_button_src.Height(), SWP_NOZORDER);
		pWnd_button_res->GetWindowRect(&rect_button_res);
		ScreenToClient(&rect_button_res);
		pWnd_button_start->SetWindowPos(NULL, rect_tab_mode.right + 20, rect_button_src.bottom + (rect_button_res.top - rect_button_src.bottom - rect_button_src.Height()) / 2, rect_button_src.Width(), rect_button_src.Height(), SWP_NOZORDER);
		pWnd_button_start->GetWindowRect(&rect_button_start);
		ScreenToClient(&rect_button_start);
		pWnd_button_single_step_left->SetWindowPos(NULL, rect_button_start.left, rect_button_start.bottom + 3, rect_button_start.Width() / 2, rect_button_start.Height() / 2, SWP_NOZORDER);
		pWnd_button_single_step_left->GetWindowRect(&rect_button_single_step_left);
		ScreenToClient(&rect_button_single_step_left);
		pWnd_button_single_step_right->SetWindowPos(NULL, rect_button_single_step_left.right, rect_button_start.bottom + 3, rect_button_start.Width() / 2, rect_button_start.Height() / 2, SWP_NOZORDER);
		pWnd_button_single_step_right->GetWindowRect(&rect_button_single_step_right);
		ScreenToClient(&rect_button_single_step_right);

		// 右部上
		pWnd_edit_show_path->SetWindowPos(NULL, rect_button_src.right + 30, client_rect.top + 10, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
		pWnd_edit_show_path->GetWindowRect(&rect_edit_show_path);
		ScreenToClient(&rect_edit_show_path);
		pWnd_s_x->SetWindowPos(NULL, rect_edit_show_path.left, rect_edit_show_path.bottom + 5, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
		pWnd_s_x->GetWindowRect(&rect_s_x);
		ScreenToClient(&rect_s_x);
		pWnd_s_y->SetWindowPos(NULL, rect_s_x.right + 10, rect_s_x.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		pWnd_s_y->GetWindowRect(&rect_s_y);
		ScreenToClient(&rect_s_y);
		GetDlgItem(IDC_STATIC_TIP)->ShowWindow(rect_s_tip.left >= rect_s_y.right);
		pWnd_s_tip->SetWindowPos(NULL, client_rect.right - rect_s_tip.Width() - 40, rect_s_y.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		pWnd_s_tip->GetWindowRect(&rect_s_tip);
		ScreenToClient(&rect_s_tip);
		pWnd_static_show_oldpic->SetWindowPos(NULL, rect_s_x.left, rect_s_x.bottom, rect_s_tip.right - rect_s_x.left, (client_rect.Height() - rect_s_x.bottom - 180) / 2, SWP_NOZORDER);
		pWnd_static_show_oldpic->GetWindowRect(&rect_static_show_oldpic);
		ScreenToClient(&rect_static_show_oldpic);
		pWnd_pic_old->SetWindowPos(NULL, rect_static_show_oldpic.left + 5, rect_static_show_oldpic.top + 25, rect_static_show_oldpic.Width() - 10, (rect_static_show_oldpic.Height() - 30), SWP_NOZORDER);
		pWnd_pic_old->GetWindowRect(&rect_pic_old);
		ScreenToClient(&rect_pic_old);
		pWnd_slider_old->SetWindowPos(NULL, rect_static_show_oldpic.left + (rect_static_show_oldpic.Width() - rect_slider_old.Width() - rect_check_zoom_old.Width() - 20) / 2, rect_static_show_oldpic.bottom + 10, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
		pWnd_slider_old->GetWindowRect(&rect_slider_old);
		ScreenToClient(&rect_slider_old);
		pWnd_check_zoom_old->SetWindowPos(NULL, rect_slider_old.right + 20, rect_slider_old.top, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
		pWnd_check_zoom_old->GetWindowRect(&rect_check_zoom_old);
		ScreenToClient(&rect_check_zoom_old);

		// 右部中
		pWnd_static_receive_pics->SetWindowPos(NULL, rect_static_show_oldpic.left, rect_static_show_oldpic.bottom + 5, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		pWnd_static_receive_pics->GetWindowRect(&rect_static_receive_pics);
		ScreenToClient(&rect_static_receive_pics);
		pWnd_static_process->SetWindowPos(NULL, rect_static_show_oldpic.left, rect_check_zoom_old.bottom + 5, rect_combo_process_1.Width() * 4 + 55, rect_combo_process_1.Height() + 30, SWP_NOZORDER);
		pWnd_static_process->GetWindowRect(&rect_static_process);
		ScreenToClient(&rect_static_process);
		pWnd_combo_process_1->SetWindowPos(NULL, rect_static_process.left + 5, rect_static_process.top + 25, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
		pWnd_combo_process_1->GetWindowRect(&rect_combo_process_1);
		ScreenToClient(&rect_combo_process_1);
		pWnd_combo_process_2->SetWindowPos(NULL, rect_combo_process_1.right + 15, rect_combo_process_1.top, rect_combo_process_1.Width(), rect_combo_process_1.Height(), SWP_NOZORDER);
		pWnd_combo_process_2->GetWindowRect(&rect_combo_process_2);
		ScreenToClient(&rect_combo_process_2);
		pWnd_combo_process_3->SetWindowPos(NULL, rect_combo_process_2.right + 15, rect_combo_process_1.top, rect_combo_process_1.Width(), rect_combo_process_1.Height(), SWP_NOZORDER);
		pWnd_combo_process_3->GetWindowRect(&rect_combo_process_3);
		ScreenToClient(&rect_combo_process_3);
		pWnd_combo_process_4->SetWindowPos(NULL, rect_combo_process_3.right + 15, rect_combo_process_1.top, rect_combo_process_1.Width(), rect_combo_process_1.Height(), SWP_NOZORDER);
		pWnd_combo_process_4->GetWindowRect(&rect_combo_process_4);
		ScreenToClient(&rect_combo_process_4);
		GetDlgItem(IDC_LIST2)->ShowWindow(rect_list_pertrans.left >= rect_static_process.right);
		pWnd_list_pertrans->SetWindowPos(NULL, rect_static_show_oldpic.right - rect_list_pertrans.Width(), rect_check_zoom_old.bottom + 5, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		pWnd_list_pertrans->GetWindowRect(&rect_list_pertrans);
		ScreenToClient(&rect_list_pertrans);
		GetDlgItem(IDC_STATIC_ZOOM_PER)->ShowWindow((rect_static_show_oldpic.Width() - rect_list_pertrans.Width() - rect_static_process.Width() - rect_slider_zoom_perspective.Width() - 20) / 2 >= 0);
		GetDlgItem(IDC_SLIDER3)->ShowWindow((rect_static_show_oldpic.Width() - rect_list_pertrans.Width() - rect_static_process.Width() - rect_slider_zoom_perspective.Width() - 20) / 2 >= 0);
		pWnd_static_zoom_perspective->SetWindowPos(NULL, rect_static_process.right + (rect_static_show_oldpic.Width() - rect_list_pertrans.Width() - rect_static_process.Width() - rect_slider_zoom_perspective.Width() - 20) / 2,
			rect_static_process.top, rect_slider_zoom_perspective.Width() + 20, rect_slider_zoom_perspective.Height() + 30, SWP_NOZORDER);
		pWnd_static_zoom_perspective->GetWindowRect(&rect_static_zoom_perspective);
		ScreenToClient(&rect_static_zoom_perspective);
		pWnd_slider_zoom_perspective->SetWindowPos(NULL, rect_static_zoom_perspective.left + 5, rect_static_zoom_perspective.top + 25, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		pWnd_slider_zoom_perspective->GetWindowRect(&rect_slider_zoom_perspective);
		ScreenToClient(&rect_slider_zoom_perspective);

		// 右部下

		pWnd_static_show_newpic->SetWindowPos(NULL, rect_static_show_oldpic.left, rect_list_pertrans.bottom + 5, rect_static_show_oldpic.Width(), rect_static_show_oldpic.Height(), SWP_NOZORDER);
		pWnd_static_show_newpic->GetWindowRect(&rect_static_show_newpic);
		ScreenToClient(&rect_static_show_newpic);
		pWnd_button_veer->SetWindowPos(NULL, rect_static_show_newpic.left, rect_static_process.bottom, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		pWnd_button_veer->GetWindowRect(&rect_button_veer);
		ScreenToClient(&rect_button_veer);
		pWnd_pic_new->SetWindowPos(NULL, rect_static_show_newpic.left + 5, rect_static_show_newpic.top + 25, rect_static_show_newpic.Width() - 10, (rect_static_show_newpic.Height() - 30), SWP_NOZORDER);
		pWnd_pic_new->GetWindowRect(&rect_pic_new);
		ScreenToClient(&rect_pic_new);
		pWnd_slider_new->SetWindowPos(NULL, rect_static_show_newpic.left + (rect_static_show_newpic.Width() - rect_slider_new.Width() - rect_check_zoom_new.Width() - 20) / 2, rect_static_show_newpic.bottom + 10, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
		pWnd_slider_new->GetWindowRect(&rect_slider_new);
		ScreenToClient(&rect_slider_new);
		pWnd_check_zoom_new->SetWindowPos(NULL, rect_slider_new.right + 20, rect_slider_new.top, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
		pWnd_check_zoom_new->GetWindowRect(&rect_check_zoom_new);
		ScreenToClient(&rect_check_zoom_new);
		// 改变slider
		if (paint_img_raw.rows != 0)
		{
			int height_times = (rect_pic_new.Height() / (paint_img_raw.rows / m_slider_zoom_raw.GetPos()));
			int width_times = (rect_pic_new.Width() / (paint_img_raw.cols / m_slider_zoom_raw.GetPos()));
			int max_val = height_times > width_times ? height_times : width_times;
			max_val--;
			max_zoom = max_val;
			if (max_zoom > 0 && m_zoom_val_raw > max_zoom)
			{
				int is_zoom_pixel_raw = ((CButton*)GetDlgItem(IDC_CHECK_PIXEL_RAW))->GetCheck();
				if (is_zoom_pixel_raw == BST_CHECKED)
				{
					cv::Mat temp = paint_img_raw;
					cv::resize(temp, temp, cv::Size(temp.cols / m_zoom_val_raw, temp.rows / m_zoom_val_raw));
					for (int i = 0; i < temp.rows; ++i)
					{
						for (int j = 0; j < temp.cols; ++j)
						{
							temp.at<uchar>(i, j) = paint_img_raw.at<uchar>(i * m_zoom_val_raw, j * m_zoom_val_raw);
						}
					}

					cv::resize(paint_img_raw, paint_img_raw, cv::Size(paint_img_raw.cols * max_zoom / (m_zoom_val_raw), paint_img_raw.rows * max_zoom / (m_zoom_val_raw)));
					for (int i = 0; i < paint_img_raw.rows; ++i)
					{
						for (int j = 0; j < paint_img_raw.cols; ++j)
						{
							paint_img_raw.at<uchar>(i, j) = temp.at<uchar>(i / max_zoom, j / max_zoom);
						}
					}
				}
				else
				{
					cv::resize(paint_img_raw, paint_img_raw, cv::Size(paint_img_raw.cols * max_zoom / (m_zoom_val_raw), paint_img_raw.rows * max_zoom / (m_zoom_val_raw)));
				}
				m_zoom_val_raw = max_zoom;
			}
			if (max_zoom > 0 && m_zoom_val_process > max_zoom)
			{
				int is_zoom_pixel_process = ((CButton*)GetDlgItem(IDC_CHECK_PIXEL_PROCESS))->GetCheck();
				if (is_zoom_pixel_process == BST_CHECKED)
				{
					cv::Mat temp = paint_img_process;
					cv::resize(temp, temp, cv::Size(temp.cols / m_zoom_val_process, temp.rows / m_zoom_val_process));
					for (int i = 0; i < temp.rows; ++i)
					{
						for (int j = 0; j < temp.cols; ++j)
						{
							temp.at<uchar>(i, j) = paint_img_process.at<uchar>(i * m_zoom_val_process, j * m_zoom_val_process);
						}
					}

					cv::resize(paint_img_process, paint_img_process, cv::Size(paint_img_process.cols * max_zoom / (m_zoom_val_process), paint_img_process.rows * max_zoom / (m_zoom_val_process)));
					for (int i = 0; i < paint_img_process.rows; ++i)
					{
						for (int j = 0; j < paint_img_process.cols; ++j)
						{
							paint_img_process.at<uchar>(i, j) = temp.at<uchar>(i / max_zoom, j / max_zoom);
						}
					}
				}
				else
				{
					cv::resize(paint_img_process, paint_img_process, cv::Size(paint_img_process.cols * max_zoom / (m_zoom_val_process), paint_img_process.rows * max_zoom / (m_zoom_val_process)));

				}
				m_zoom_val_process = max_zoom;
			}
			m_slider_zoom_raw.SetRange(1, max_val);
			m_slider_zoom_raw.SetTicFreq(1);

			m_slider_zoom_process.SetRange(1, max_val);
			m_slider_zoom_process.SetTicFreq(1);

			m_slider_zoom_perspective.SetRange(1, max_val);
			m_slider_zoom_perspective.SetTicFreq(1);
		}
	}
	// 以上串口配置区域位置调整
}

void CImgProcess::OnBnClickedButtonVeer()
{
	if (veer_show_dlg != NULL)
	{
		veer_show_dlg->~CVeerShow();
		delete veer_show_dlg;
		veer_show_dlg = NULL;
		return;
	}
	veer_show_dlg = new CVeerShow(this, &veer_show_dlg, &slope);
	veer_show_dlg->Create(IDD_VEER);
	veer_show_dlg->ShowWindow(SW_SHOWNORMAL);
}

BOOL CImgProcess::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CRect rect_raw;
	GetDlgItem(IDC_STATIC_PIC)->GetWindowRect(&rect_raw); //获取被选中的控件大小
	CRect rect_process;
	GetDlgItem(IDC_STATIC_PROCESS_PIC)->GetWindowRect(&rect_process); //获取被选中的控件大小
	CRect rect_border_raw;
	GetDlgItem(IDC_STATIC_SHOW_PIC)->GetWindowRect(&rect_border_raw);
	CRect rect_border_process;
	GetDlgItem(IDC_STATIC_SHOW_NEW)->GetWindowRect(&rect_border_process);
	if (pt.x > rect_raw.left && pt.x < rect_raw.right &&
		pt.y > rect_raw.top && pt.y < rect_raw.bottom)
	{
		ScreenToClient(&rect_raw);  //转化为客户区坐标
		//InvalidateRect(&rect_raw);
		ScreenToClient(&rect_border_raw);
		InvalidateRect(&rect_border_raw);
		if (zDelta > 0 && m_zoom_val_raw < max_zoom)
		{
			int is_zoom_pixel_raw = ((CButton*)GetDlgItem(IDC_CHECK_PIXEL_RAW))->GetCheck();
			if (is_zoom_pixel_raw == BST_CHECKED)
			{
				cv::Mat temp = paint_img_raw;
				cv::resize(temp, temp, cv::Size(temp.cols / m_zoom_val_raw, temp.rows / m_zoom_val_raw));
				for (int i = 0; i < temp.rows; ++i)
				{
					for (int j = 0; j < temp.cols; ++j)
					{
						temp.at<uchar>(i, j) = paint_img_raw.at<uchar>(i * m_zoom_val_raw, j * m_zoom_val_raw);
					}
				}

				cv::resize(paint_img_raw, paint_img_raw, cv::Size(paint_img_raw.cols * (m_zoom_val_raw + 1) / (m_zoom_val_raw), paint_img_raw.rows * (m_zoom_val_raw + 1) / (m_zoom_val_raw)));
				for (int i = 0; i < paint_img_raw.rows; ++i)
				{
					for (int j = 0; j < paint_img_raw.cols; ++j)
					{
						paint_img_raw.at<uchar>(i, j) = temp.at<uchar>(i / (m_zoom_val_raw + 1), j / (m_zoom_val_raw + 1));
					}
				}
			}
			else
			{
				cv::resize(paint_img_raw, paint_img_raw, cv::Size(paint_img_raw.cols * (m_zoom_val_raw + 1) / (m_zoom_val_raw), paint_img_raw.rows * (m_zoom_val_raw + 1) / (m_zoom_val_raw)));
			}
			m_zoom_val_raw++;
			m_slider_zoom_raw.SetPos(m_zoom_val_raw);
			OnPaint();
		}
		else if (zDelta < 0 && m_zoom_val_raw > 1)
		{
			int is_zoom_pixel_raw = ((CButton*)GetDlgItem(IDC_CHECK_PIXEL_RAW))->GetCheck();
			if (is_zoom_pixel_raw == BST_CHECKED)
			{
				cv::Mat temp = paint_img_raw;
				cv::resize(temp, temp, cv::Size(temp.cols / m_zoom_val_raw, temp.rows / m_zoom_val_raw));
				for (int i = 0; i < temp.rows; ++i)
				{
					for (int j = 0; j < temp.cols; ++j)
					{
						temp.at<uchar>(i, j) = paint_img_raw.at<uchar>(i * m_zoom_val_raw, j * m_zoom_val_raw);
					}
				}

				cv::resize(paint_img_raw, paint_img_raw, cv::Size(paint_img_raw.cols * (m_zoom_val_raw - 1) / (m_zoom_val_raw), paint_img_raw.rows * (m_zoom_val_raw - 1) / (m_zoom_val_raw)));
				for (int i = 0; i < paint_img_raw.rows; ++i)
				{
					for (int j = 0; j < paint_img_raw.cols; ++j)
					{
						paint_img_raw.at<uchar>(i, j) = temp.at<uchar>(i / (m_zoom_val_raw - 1), j / (m_zoom_val_raw - 1));
					}
				}
			}
			else
			{
				cv::resize(paint_img_raw, paint_img_raw, cv::Size(paint_img_raw.cols * (m_zoom_val_raw - 1) / (m_zoom_val_raw), paint_img_raw.rows * (m_zoom_val_raw - 1) / (m_zoom_val_raw)));
			}
			m_zoom_val_raw--;
			m_slider_zoom_raw.SetPos(m_zoom_val_raw);
			OnPaint();
		}
	}
	else if (pt.x > rect_process.left && pt.x < rect_process.right &&
		pt.y > rect_process.top && pt.y < rect_process.bottom)
	{
		ScreenToClient(&rect_process);  //转化为客户区坐标
		//InvalidateRect(&rect_process);
		ScreenToClient(&rect_border_process);
		InvalidateRect(&rect_border_process);
		if (zDelta > 0 && m_zoom_val_process < max_zoom / m_zoom_val_perspective)
		{
			int is_zoom_pixel_process = ((CButton*)GetDlgItem(IDC_CHECK_PIXEL_PROCESS))->GetCheck();
			if (is_zoom_pixel_process == BST_CHECKED)
			{
				cv::Mat temp = paint_img_process;
				cv::resize(temp, temp, cv::Size(temp.cols / m_zoom_val_process, temp.rows / m_zoom_val_process));
				for (int i = 0; i < temp.rows; ++i)
				{
					for (int j = 0; j < temp.cols; ++j)
					{
						temp.at<uchar>(i, j) = paint_img_process.at<uchar>(i * m_zoom_val_process, j * m_zoom_val_process);
					}
				}

				cv::resize(paint_img_process, paint_img_process, cv::Size(paint_img_process.cols * (m_zoom_val_process + 1) / (m_zoom_val_process), paint_img_process.rows * (m_zoom_val_process + 1) / (m_zoom_val_process)));
				for (int i = 0; i < paint_img_process.rows; ++i)
				{
					for (int j = 0; j < paint_img_process.cols; ++j)
					{
						paint_img_process.at<uchar>(i, j) = temp.at<uchar>(i / (m_zoom_val_process + 1), j / (m_zoom_val_process + 1));
					}
				}
			}
			else
			{
				cv::resize(paint_img_process, paint_img_process, cv::Size(paint_img_process.cols * (m_zoom_val_process + 1) / (m_zoom_val_process), paint_img_process.rows * (m_zoom_val_process + 1) / (m_zoom_val_process)));

			}
			m_zoom_val_process++;
			m_slider_zoom_process.SetPos(m_zoom_val_process);
			OnPaint();
		}
		else if (zDelta < 0 && m_zoom_val_process > 1)
		{
			int is_zoom_pixel_process = ((CButton*)GetDlgItem(IDC_CHECK_PIXEL_PROCESS))->GetCheck();
			if (is_zoom_pixel_process == BST_CHECKED)
			{
				cv::Mat temp = paint_img_process;
				cv::resize(temp, temp, cv::Size(temp.cols / m_zoom_val_process, temp.rows / m_zoom_val_process));
				for (int i = 0; i < temp.rows; ++i)
				{
					for (int j = 0; j < temp.cols; ++j)
					{
						temp.at<uchar>(i, j) = paint_img_process.at<uchar>(i * m_zoom_val_process, j * m_zoom_val_process);
					}
				}

				cv::resize(paint_img_process, paint_img_process, cv::Size(paint_img_process.cols * (m_zoom_val_process - 1) / (m_zoom_val_process), paint_img_process.rows * (m_zoom_val_process - 1) / (m_zoom_val_process)));
				for (int i = 0; i < paint_img_process.rows; ++i)
				{
					for (int j = 0; j < paint_img_process.cols; ++j)
					{
						paint_img_process.at<uchar>(i, j) = temp.at<uchar>(i / (m_zoom_val_process - 1), j / (m_zoom_val_process - 1));
					}
				}
			}
			else
			{
				cv::resize(paint_img_process, paint_img_process, cv::Size(paint_img_process.cols * (m_zoom_val_process - 1) / (m_zoom_val_process), paint_img_process.rows * (m_zoom_val_process - 1) / (m_zoom_val_process)));
			}
			m_zoom_val_process--;
			m_slider_zoom_process.SetPos(m_zoom_val_process);
			OnPaint();
		}
	}

	return CDialogEx::OnMouseWheel(nFlags, zDelta, pt);
}


void CImgProcess::OnBnClickedButtonSingleStepLeft()
{
	is_single_step_pixel_src = true;
	// 求取透视变换矩阵
	GetPerspectiveTransformMat();
	// 更新list_control
	CString str_temp1, str_temp2, str_temp3;
	for (int i = 0; i < 3; ++i)
	{
		str_temp1.Format(_T("%lf"), perspective_transform_mat.at<double>(i, 0));
		str_temp2.Format(_T("%lf"), perspective_transform_mat.at<double>(i, 1));
		str_temp3.Format(_T("%lf"), perspective_transform_mat.at<double>(i, 2));
		m_list_transform.SetItemText(i, 0, str_temp1);
		m_list_transform.SetItemText(i, 1, str_temp2);
		m_list_transform.SetItemText(i, 2, str_temp3);
	}
	GetPixelPicInfo();
	if (is_last_right_step)
	{
		pixel_src_file_path_selected_index -= 2;
		is_last_right_step = false;
	}
	is_last_left_step = true;
	if (pixel_src_file_path_selected_index < 0 || pixel_src_file_path_selected_index == pixel_src_file_path_selected.size())
	{
		is_single_step_pixel_src = false;
		is_last_left_step = false;
		pixel_src_file_path_selected_index = 0;
		::MessageBox(NULL, _T("无可加载资源!"), _T("提示"), 0);
		// 更新 IDC_PIXEL_SRC_PATH 显示
		SetDlgItemTextW(IDC_PIXEL_SRC_PATH, _T(""));
		return;
	}

	RefleshPicPixel(this);
	pixel_src_file_path_selected_index--;
	is_single_step_pixel_src = false;
}


void CImgProcess::OnBnClickedButtonSingleStepRight()
{
	is_single_step_pixel_src = true;
	// 求取透视变换矩阵
	GetPerspectiveTransformMat();
	// 更新list_control
	CString str_temp1, str_temp2, str_temp3;
	for (int i = 0; i < 3; ++i)
	{
		str_temp1.Format(_T("%lf"), perspective_transform_mat.at<double>(i, 0));
		str_temp2.Format(_T("%lf"), perspective_transform_mat.at<double>(i, 1));
		str_temp3.Format(_T("%lf"), perspective_transform_mat.at<double>(i, 2));
		m_list_transform.SetItemText(i, 0, str_temp1);
		m_list_transform.SetItemText(i, 1, str_temp2);
		m_list_transform.SetItemText(i, 2, str_temp3);
	}
	GetPixelPicInfo();
	if (is_last_left_step)
	{
		pixel_src_file_path_selected_index += 2;
		is_last_left_step = false;
	}
	is_last_right_step = true;
	if (pixel_src_file_path_selected_index >= pixel_src_file_path_selected.size())
	{
		is_single_step_pixel_src = false;
		is_last_right_step = false;
		pixel_src_file_path_selected_index = 0;
		::MessageBox(NULL, _T("无可加载资源!"), _T("提示"), 0);
		// 更新 IDC_PIXEL_SRC_PATH 显示
		SetDlgItemTextW(IDC_PIXEL_SRC_PATH, _T(""));
		return;
	}
	RefleshPicPixel(this);
	pixel_src_file_path_selected_index++;
	is_single_step_pixel_src = false;
}


void CImgProcess::OnTcnSelchangeTabMode(NMHDR* pNMHDR, LRESULT* pResult)
{
	int num = m_tab_mode.GetCurSel();//获取点击了哪一个页面
	switch (num)
	{
	case 0:
		forms_cam_uart.ShowWindow(SW_SHOW);
		forms_cam_net_server.ShowWindow(SW_HIDE);
		forms_cam_net_client.ShowWindow(SW_HIDE);
		break;
	case 1:
		forms_cam_uart.ShowWindow(SW_HIDE);
		forms_cam_net_server.ShowWindow(SW_SHOW);
		forms_cam_net_client.ShowWindow(SW_HIDE);
		break;
	case 2:
		forms_cam_uart.ShowWindow(SW_HIDE);
		forms_cam_net_server.ShowWindow(SW_HIDE);
		forms_cam_net_client.ShowWindow(SW_SHOW);
		break;
	}
	*pResult = 0;
}

static std::string DatetimeToString(tm tm_in)
{
	tm* tm_ = &tm_in;						  // 将time_t格式转换为tm结构体
	int year, month, day, hour, minute, second;// 定义时间的各个int临时变量。
	year = tm_->tm_year + 1900;                // 临时变量，年，由于tm结构体存储的是从1900年开始的时间，所以临时变量int为tm_year加上1900。
	month = tm_->tm_mon + 1;                   // 临时变量，月，由于tm结构体的月份存储范围为0-11，所以临时变量int为tm_mon加上1。
	day = tm_->tm_mday;
	hour = tm_->tm_hour;
	minute = tm_->tm_min;
	second = tm_->tm_sec;

	char s[20];                                // 定义总日期时间char*变量。
	sprintf(s, "%04d_%02d_%02d_%02d_%02d_%02d", year, month, day, hour, minute, second);// 将年月日时分秒合并。
	std::string str(s);                       // 定义string变量，并将总日期时间char*变量作为构造函数的参数传入。
	return std::move(str);                    // 返回转换日期时间后的string变量。
}

// 向文件写入字符串
int WriteStringToFile(const std::string& file_name, const std::string str)
{
	std::ofstream	os_write(file_name, std::ios::out);
	os_write << str;
	os_write.close();
	return 0;
}


// 一次性获取文件字符串
void GetAllContent(const std::string& file_name, std::string& resstr)
{
	std::ifstream ifs(file_name.c_str());
	std::istreambuf_iterator<char> begin(ifs);
	std::istreambuf_iterator<char> end;
	std::string str(begin, end);
	resstr = str;
	ifs.close();
}

// 图像原始数据处理
void PicDataProcess(void* ptr_param)
{
	CImgProcess* ptr = reinterpret_cast<CImgProcess*>(reinterpret_cast<ProcessSoureDataFuncParam*>(ptr_param)->form);
	bool& is_stop_this_func = reinterpret_cast<ProcessSoureDataFuncParam*>(ptr_param)->is_stop;
	// 帧头0x01 0xFE，帧尾: 0xFE 0x01
	char pic_head_chars[2] = { 0x01,0xFE };
	char pic_tail_chars[2] = { 0xFE,0x01 };
	ProcessPicStatus status = FINDIND_FIRST_HEAD_ELE;

	PicData* pProcessing_pic_data = new PicData((size_t)ptr->pic_size + 10);

	// 开启窗体处理图像函数线程
	ptr->refresh_pic_func_param.form = ptr;
	ptr->refresh_pic_func_param.is_stop = false;
	unsigned int thread_id;
	HANDLE handle_refresh_pic = (HANDLE)_beginthreadex(NULL, 0, RefleshPic, (void*)&(ptr->refresh_pic_func_param), 0, &thread_id);
	while (!is_stop_this_func)
	{
		while (ptr->raw_pic_data.empty())
		{
			Sleep(100);
			if (is_stop_this_func)
			{
				return;
			}
		}

		unsigned char* pic_data = pProcessing_pic_data->GetArray();
		unsigned char* ptr_raw_data_array = ptr->raw_pic_data.front()->GetArray();
		size_t size_ = ptr->raw_pic_data.front()->GetSize();
		for (size_t i = 0; i < size_; ++i)
		{
			// 图片错误, 丢弃
			if (pProcessing_pic_data->GetSize() > (size_t)ptr->pic_size + 8)
			{
				status = FINDIND_FIRST_HEAD_ELE;
				pProcessing_pic_data->SetSize(0);
			}
			switch (status)
			{
			case FINDIND_FIRST_HEAD_ELE:
			{
				if (ptr_raw_data_array[i] == 0x01)
				{
					status = FINDIND_SECOND_HEAD_ELE;
				}
				break;
			}
			case FINDIND_SECOND_HEAD_ELE:
			{
				if (ptr_raw_data_array[i] == 0xFE)
				{
					status = FINDING_FIRST_TAIL_ELE;
				}
				else if (ptr_raw_data_array[i] == 0x01)
				{
					status = FINDIND_SECOND_HEAD_ELE;
				}
				else
				{
					status = FINDIND_FIRST_HEAD_ELE;
				}
				break;
			}
			case FINDING_FIRST_TAIL_ELE:
			{
				// 顺序无误. 若觉疑惑, 移视 FINDING_SECOND_TAIL_ELE
				size_t ss = pProcessing_pic_data->GetSize();
				pic_data[ss++] = ptr_raw_data_array[i];
				pProcessing_pic_data->SetSize(ss);
				if (ptr_raw_data_array[i] == 0xFE)
				{
					status = FINDING_SECOND_TAIL_ELE;
					break;
				}
				break;
			}
			case FINDING_SECOND_TAIL_ELE:
			{
				if (ptr_raw_data_array[i] == 0x01)
				{
					status = FINDIND_FIRST_HEAD_ELE;
					pProcessing_pic_data->SetSize(pProcessing_pic_data->GetSize() - 1);
					ptr->pic_data.push(pProcessing_pic_data);
					pProcessing_pic_data = new PicData((size_t)ptr->pic_size + 10);
					pProcessing_pic_data->SetSize(0);
				}
				else if (ptr_raw_data_array[i] == 0xFE)
				{
					size_t ss = pProcessing_pic_data->GetSize();
					pic_data[ss++] = ptr_raw_data_array[i];
					pProcessing_pic_data->SetSize(ss);
					status = FINDING_SECOND_TAIL_ELE;
				}
				else
				{
					size_t ss = pProcessing_pic_data->GetSize();
					pic_data[ss++] = ptr_raw_data_array[i];
					pProcessing_pic_data->SetSize(ss);
					status = FINDING_FIRST_TAIL_ELE;
				}
				break;
			}
			default:
				break;
			}
		}
		ptr->raw_pic_data.front()->Release();
		delete ptr->raw_pic_data.front();
		ptr->raw_pic_data.pop();
	}
	delete pProcessing_pic_data;
}

void UpdateCSCCData(void* ptr_param)
{
	CImgProcess* ptr = reinterpret_cast<CImgProcess*>(ptr_param);
	// 更新软件输入文件信息
	ptr->WriteOperationInfoToFile();
	// 求取透视变换矩阵
	ptr->GetPerspectiveTransformMat();
	// 更新透视变换矩阵
	CString str_temp1, str_temp2, str_temp3;
	for (int i = 0; i < 3; ++i)
	{
		str_temp1.Format(_T("%lf"), ptr->perspective_transform_mat.at<double>(i, 0));
		str_temp2.Format(_T("%lf"), ptr->perspective_transform_mat.at<double>(i, 1));
		str_temp3.Format(_T("%lf"), ptr->perspective_transform_mat.at<double>(i, 2));
		ptr->m_list_transform.SetItemText(i, 0, str_temp1);
		ptr->m_list_transform.SetItemText(i, 1, str_temp2);
		ptr->m_list_transform.SetItemText(i, 2, str_temp3);
	}
	// 更新图像信息
	CString size_;
	ptr->GetDlgItemTextW(IDC_EDIT_WIDTH, size_);
	int width_size = _ttoi(size_);
	ptr->GetDlgItemTextW(IDC_EDIT_HEIGTH, size_);
	int height_size = _ttoi(size_);
	ptr->pic_size = width_size * height_size;
	ptr->pic_height = height_size;
	ptr->pic_width = width_size;
}
void ClearCSCCDate(void* ptr_param)
{
	CImgProcess* ptr = reinterpret_cast<CImgProcess*>(ptr_param);
	while (!ptr->raw_pic_data.empty())
	{
		ptr->raw_pic_data.front()->Release();
		delete ptr->raw_pic_data.front();
		ptr->raw_pic_data.pop();
	}
	while (!ptr->pic_data.empty())
	{
		ptr->pic_data.front()->Release();
		delete ptr->pic_data.front();
		ptr->pic_data.pop();
	}
}