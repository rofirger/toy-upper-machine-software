#pragma once
#include "uart.h"
#include "timer.h"
#include <opencv2/opencv.hpp>
#include <opencv2/core/utils/logger.hpp>
#include "user_process.h"
#include <string>
#include "CFileLook.h"
#include "CVeerShow.h"
#include "CUart.h"
#include "CNetClient.h"
#include "CNetServer.h"
#include "DataSrcParam.hpp"
#include "queue_cache.h"
#include "to_file.hpp"

static std::string DatetimeToString(tm tm_in);
// 向文件写入字符串
int WriteStringToFile(const std::string& file_name, const std::string str);
// 一次性获取文件字符串
void GetAllContent(const std::string& file_name, std::string& resstr);
// 刷新窗体图片区图片
unsigned __stdcall RefleshPic(LPVOID lpParam);
// 像素图操作域“开始”按钮对窗体控件的使能
void EnableControlForPixelSrc(void* ptr_param_cscc, bool true_or_false);
// 串口数据源、网络数据源“开始”按钮对窗体控件的使能
void EnableControlForDataSrc(void* ptr_param_cscc, bool true_or_false);
// 从串口数据源、网络数据源中提取图片数据
void PicDataProcess(void* ptr_param);
// 串口数据源、网络数据源“开始”按钮执行前对 CImgProcess 类的数据进行更新
void UpdateCSCCData(void* ptr_param);
// 串口数据源、网络数据源“开始”按钮执行完成后对 CImgProcess 类的数据进行清除更新等操作
void ClearCSCCDate(void* ptr_param);
typedef struct AuxiliaryLinesStruct
{
	LineArray* pleft_line;
	LineArray* pmid_line;
	LineArray* pright_line;
}AuxiliaryLinesStruct;

class CDragListCtrl :public CListCtrl
{
private:
	int m_nSelItem;
	CImageList* m_pDragImageList;
	bool m_bDragging;
public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnLvnBegindrag(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};

class CImgProcess : public CDialogEx
{
	DECLARE_DYNAMIC(CImgProcess)
public:
	CImgProcess(CWnd* pParent = nullptr);   // 标准构造函数
	void MainDlgDropFiles(const std::vector<CString>& main_dlg_drops_files);
	virtual ~CImgProcess();
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_IMGPROCESS };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

private:
	// 辅助线模块
	LineArray left_line;
	LineArray mid_line;
	LineArray right_line;
	LineArray perspective_left_line;
	LineArray perspective_mid_line;
	LineArray perspective_right_line;
public:
	// 与辅助线相关函数
	void InitAuxiliaryLines(LineArray& _line, Pos* _array_ptr = nullptr, short _size = 0);
	AuxiliaryLinesStruct GetAuxiliaryLines();
	AuxiliaryLinesStruct GetAuxiliaryPerspectiveLines();
	void DeleteAuxiliaryPerspectiveLines()
	{
		LineArray* temp_line = GetAuxiliaryPerspectiveLines().pleft_line;
		if (temp_line->_array != nullptr && temp_line->_size > 0)
		{
			delete[]temp_line->_array;
			temp_line->_array = nullptr;
			temp_line->_index = 0;
			temp_line->_size = 0;
		}
		temp_line = GetAuxiliaryPerspectiveLines().pmid_line;
		if (temp_line->_array != nullptr && temp_line->_size > 0)
		{
			delete[]temp_line->_array;
			temp_line->_array = nullptr;
			temp_line->_index = 0;
			temp_line->_size = 0;
		}
		temp_line = GetAuxiliaryPerspectiveLines().pright_line;
		if (temp_line->_array != nullptr && temp_line->_size > 0)
		{
			delete[]temp_line->_array;
			temp_line->_array = nullptr;
			temp_line->_index = 0;
			temp_line->_size = 0;
		}
	}
private:
	// 图像处理选择情况
	// 图像处理选择情况, 目前四种，通过修改数值以添加新的处理
	size_t process_sel[4];
public:
	void ZeroProcessSel() { process_sel[0] = 0, process_sel[1] = 1, process_sel[2] = 2, process_sel[3] = 3; }
	void SetProcessSel(const size_t _index, const size_t _sel) { process_sel[_index] = _sel; }
	size_t GetProcessSel(const size_t _index) { return process_sel[_index]; }
	bool IsHaveProcessSel(const size_t _sel)
	{
		for (int i = 0; i < 4; ++i)
			if (process_sel[i] == _sel)
				return true;
	}
public:
	CSerialPort mySerialPort;
	Timer refresh_timer;
	Timer reconnect_timer;
	Queue<QueueCacheDataType*> raw_pic_data;
	size_t raw_pic_data_ele_size = 1000;
	// 供RefreshPic 及 PicDataProcess使用
	Queue<QueueCacheDataType*>pic_data;
	ProcessSoureDataFuncParam process_pic_func_param{ this,false };
	RefreshPicFuncParam refresh_pic_func_param{ this,false };
	// 数据源参数类
	DataSrcParam data_src_param;
	// 接收到的图片数
	size_t num_receive_pics = 0;;
public:
	CComboBox m_combo_pictype;
	CEdit m_edit_width;
	CEdit m_edit_heigth;
	CEdit m_edit_thval;
	CSpinButtonCtrl m_spin;
	CComboBox m_combo_process_one;
	CComboBox m_combo_process_two;
	CComboBox m_combo_process_three;
	CComboBox m_combo_process_four;
	CListCtrl m_list_transform;
public:
	unsigned int pic_size;
	int pic_width;
	int pic_height;
	double slope;

	volatile bool IS_STOP_RECONNECT = false;
	cv::Mat& GetPerspectiveTransformMat();
	// void EnableControl(bool true_or_false);
	// 图像处理
	// 二值化
	unsigned char** InternalBinaryProcess(unsigned char** pixel_mat_param, cv::Mat& img_process_param);
	// 寻找辅助线
	unsigned char** InternalAuxiliaryProcess(unsigned char** pixel_mat_param, cv::Mat& img_process_param);
	// 透视变换
	unsigned char** InternalPerspectiveProcess(unsigned char** pixel_mat_param, cv::Mat& img_process_param, int max_zoom_val_param);
	// 自定义
	UserProcessRet InternalUserProcess(unsigned char** pixel_mat_param, cv::Mat& img_process_param, int max_zoom_val_param, bool& is_show_left_line, bool& is_show_mid_line, bool& is_show_right_line);
public:
	cv::Mat perspective_transform_mat;
	cv::Mat paint_img_process;
	cv::Mat paint_img_raw;
	CSliderCtrl m_slider_zoom_process;
	CSliderCtrl m_slider_zoom_raw;
	CSliderCtrl m_slider_zoom_perspective;
protected:
	rofirger::ToFile _data_to_file;
public:
	void WriteImgDataToFile(const std::string file_dic_path, const std::string  data, const int width, const int height, const int kind)
	{
		_data_to_file.InsertImgFile(file_dic_path, data, width, height, kind);
	}
public:
	// 图片及坐标
	int m_zoom_val_raw = 1;
	int m_zoom_val_process = 1;
	int m_zoom_val_perspective = 1;
	int m_pos_x = 0;
	int m_pos_y = 0;
	// 指代图片的左上角坐标, 相对于图片框
	int m_pos_raw_x_zero = 0;
	int m_pos_raw_y_zero = 0;
	int m_pos_process_x_zero = 0;
	int m_pos_process_y_zero = 0;
	int max_zoom = 1;

private:
	CToolTipCtrl m_toolTip;
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnNMClickList2(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnNMCustomdrawSlider2(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMCustomdrawSliderPerspective(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMCustomdrawSlider1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnPaint();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedSaveRaw();
	afx_msg void OnBnClickedSaveProcess();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnBnClickedButton7();
	afx_msg void OnBnClickedButton8();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedButtonVeer();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnBnClickedButtonSingleStepLeft();
	afx_msg void OnBnClickedButtonSingleStepRight();
	afx_msg void OnTcnSelchangeTabMode(NMHDR* pNMHDR, LRESULT* pResult);

public:
	std::string path_raw;
	std::string path_process;

	// 为用户自定义库提供输入输出接口
	CEdit m_user_output;
	std::string user_output;
	CEdit m_user_input;
	std::string user_input;
	// 为用户提供RGB绘点
	std::vector<std::vector<UserRGB>> user_rgb_mat_vec;

public:
	/* 该区域与 CPixelSrc 联系 */

	// 文件夹路径
	CString pixel_src_path;
	// 仅文件名
	std::vector<CString>pixel_src_file_path;
	// 是否选择像素源
	bool is_pixel_src_checked = false;
	// 间隔(ms)
	size_t interval = 1000;
	// 已选中像素文件，仅文件名
	std::vector<CString>pixel_src_file_path_selected;
	int pixel_src_file_path_selected_index = 0;
	// 是否开启
	bool is_start_pixel_src = false;
	bool is_single_step_pixel_src = false;
	bool is_last_left_step = false;
	bool is_last_right_step = false;
	// 存储图片信息(高，宽，图片类型)
	std::vector<PICINFO>pic_info;
	// 获取像素文件关于图片(除像素外)的信息
	void GetPixelPicInfo();
public:
	// 文件拖曳
	CFileLook* file_look = NULL;
	bool is_close_look = true;;
	// 完整路径
	std::vector<CString>drop_files;
private:
	// 改变窗口大小
	// 	窗口配置部分
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
	// 左下角区域
	CRect rect_static_binary;
	CRect rect_pictype;
	CRect rect_s_width;
	CRect rect_edit_width;
	CRect rect_s_height;
	CRect rect_edit_height;
	CRect rect_s_threshold;
	CRect rect_edit_threshold;
	CRect rect_spin_threshold;

	CRect rect_static_perspective;
	CRect rect_static_old;
	CRect rect_x_1;
	CRect rect_y_1;
	CRect rect_x_2;
	CRect rect_y_2;
	CRect rect_x_3;
	CRect rect_y_3;
	CRect rect_x_4;
	CRect rect_y_4;
	CRect rect_static_new;
	CRect rect_x_11;
	CRect rect_y_11;
	CRect rect_x_22;
	CRect rect_y_22;
	CRect rect_x_33;
	CRect rect_y_33;
	CRect rect_x_44;
	CRect rect_y_44;

	CRect rect_static_picsave;
	CRect rect_button_trans;
	CRect rect_check_save_old;
	CRect rect_check_save_new;
	CRect rect_edit_save_old_path;
	CRect rect_edit_save_new_path;

	CRect rect_edit_input;
	CRect rect_edit_ouput;

	// 中部
	CRect rect_button_src;
	CRect rect_button_start;
	CRect rect_button_res;
	CRect rect_button_single_step_left;
	CRect rect_button_single_step_right;

	// 右部上
	CRect rect_edit_show_path;
	CRect rect_s_x;
	CRect rect_s_y;
	CRect rect_s_tip;
	CRect rect_static_show_oldpic;
	CRect rect_pic_old;
	CRect rect_slider_old;
	CRect rect_check_zoom_old;

	// 右部中
	CRect rect_static_receive_pics;
	CRect rect_static_process;
	CRect rect_combo_process_1;
	CRect rect_combo_process_2;
	CRect rect_combo_process_3;
	CRect rect_combo_process_4;
	CRect rect_static_zoom_perspective;
	CRect rect_slider_zoom_perspective;
	CRect rect_list_pertrans;

	// 右部下
	CRect rect_button_veer;
	CRect rect_static_show_newpic;
	CRect rect_pic_new;
	CRect rect_slider_new;
	CRect rect_check_zoom_new;
private:
	CVeerShow* veer_show_dlg = NULL;
public:
	void GetFileFromDirectory(CString csDirPath, std::vector<CString>& vctPath);
	void ReadPixelSelectedFile(std::string file_name, std::vector<CString>& vec);
	// UARTDisplayDlg的Onclose()调用
	void WriteOperationInfoToFile();
	void ReadOperationInfoFromFile();
	CTabCtrl m_tab_mode;
public:													//tab 部分
	CUart forms_cam_uart;
	CNetServer forms_cam_net_server;
	CNetClient forms_cam_net_client;
};
typedef struct tagBITMAPINFO_X
{
	BITMAPINFOHEADER  bmiHeader;
	RGBQUAD           bmiColors[256];
} BITMAPINFO_X;
