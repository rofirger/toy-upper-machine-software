#pragma once
#include "uart.h"
#include "timer.h"
#include "queue_cache.h"
#include "DataSrcParam.hpp"
#include <queue>
unsigned __stdcall DataAcquisitionUart(LPVOID lpParam);
unsigned __stdcall ProcessFuncUart(LPVOID lpParam);
typedef struct UartDataAcParam
{
	void* forms;
	CSerialPort* ptr_serial_port;
	Queue<QueueCacheDataType*>* ptr_queue_data;
	size_t data_queue_ele_default_size;
}UartDataAcParam;

//using RefreshFuncParam = UartDataAcParam;
class CUart : public CDialogEx
{
	DECLARE_DYNAMIC(CUart)

public:
	CUart(DataSrcParam& data_src_param) :_data_src_param(data_src_param),
		data_queue_cache(data_src_param.GetDataQueue()),
		data_queue_cache_ele_default_size(data_src_param.GetDataQueueDefaultEleSize())
	{
		handle_uart = nullptr;
		handle_data_process = nullptr;
	}
	virtual ~CUart();

	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_UART };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	void ReadCom();
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	/*unsigned  (*Reflesh_Pic)(LPVOID) = nullptr;
	void* refresh_pic_func_param = nullptr;
	void(*EnableControl)(void* ptr_param_cscc, bool true_or_false) = nullptr;
	void (*UpdateParentFormData)(void*) = nullptr;
	void* update_parent_form_data_param = nullptr;
	void (*ClearParentFormData)(void*) = nullptr;
	void* clear_data_func_param = nullptr;*/
public:
	CSerialPort mySerialPort;
	DataSrcParam& _data_src_param;
	Queue<QueueCacheDataType*>& data_queue_cache;
	size_t data_queue_cache_ele_default_size;
	HANDLE handle_uart;
	HANDLE handle_data_process;
	UartDataAcParam data_ac_param;
	UartProcessFuncParam process_func_param{ this,_data_src_param.GetPtrFuncDataProThreadParam() };
public:
	CComboBox m_combo_coms;
	CComboBox m_combo_baudrate;
	CComboBox m_combo_databit;
	CComboBox m_combo_check;
	CComboBox m_combo_stopbit;

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
	CRect rect_button;
	afx_msg void OnBnClickedMainButton();
};
