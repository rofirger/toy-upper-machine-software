#pragma once
#include "uart.h"
#include "CNetClient.h"
#include "CNetServer.h"
#include "CUart.h"
#include <setupapi.h>
#include <devguid.h>
#include <regstr.h>
#include <shlwapi.h>
#include <string>
#include <malloc.h>
#include"timer.h"
#include "matrix.h"
#pragma comment(lib, "setupapi.lib")
// 串口数据源、网络数据源“开启串口”按钮对窗体控件的使能
void EnableSimpleInfoControlForDataSrc(void* ptr_param_simple_info, bool true_or_false);
// 从串口数据源、网络数据源中提取数据
void ProcessSimpleInfoData(void* ptr_param);
// 串口数据源、网络数据源“开始”按钮执行前对 CSimpleInfo 类的数据进行更新
void UpdateSimpleInfoData(void* ptr_param);
// 串口数据源、网络数据源“开始”按钮执行完成后对 CSimpleInfo 类的数据进行清除更新等操作
void ClearSimpleInfoDate(void* ptr_param);
template<typename T>
class ArrayDoubleIncrease
{
private:
	T* _array;
	size_t _malloc_size;
	// 索引小于 _now_free_index 已被占用
	size_t _now_free_index;
	void DoubleSize()
	{
		_malloc_size *= 2;
		//T* temp = reinterpret_cast<T*>(realloc(_array, _malloc_size * sizeof(T)));
		T* temp = (T*)malloc(_malloc_size * sizeof(T));
		for (size_t i = 0; i < _now_free_index; ++i)
		{
			temp[i] = _array[i];
		}
		_array = temp;
	}
public:
	ArrayDoubleIncrease(size_t _s_)
	{
		_malloc_size = _s_;
		_now_free_index = 0;
		_array = reinterpret_cast<T*>(malloc(_malloc_size * sizeof(T)));
	}
	ArrayDoubleIncrease()
	{
		_malloc_size = 100;
		_now_free_index = 0;
		_array = reinterpret_cast<T*>(malloc(_malloc_size * sizeof(T)));
	}
	size_t GetSize() { return _now_free_index; }
	void InsertData(const T* _data_, size_t _s_)
	{
		size_t inserted_size = _s_ + _now_free_index;
		if (inserted_size >= _malloc_size)
		{
			DoubleSize();
		}
		for (size_t i = _now_free_index; i < inserted_size; ++i)
		{
			_array[i] = _data_[i];
		}
	}
	void InsertZeroIntoTail() {}
	const T* GetArray() { return _array; }
	void ClearData() { _now_free_index = 0; }
	~ArrayDoubleIncrease()
	{
		free(_array);
	}
};
// 特化
template<>
class ArrayDoubleIncrease<char>
{
private:
	char* _array;
	size_t _malloc_size;
	// 索引小于 _now_free_index 已被占用
	size_t _now_free_index;
	void DoubleSize()
	{
		_malloc_size *= 2;
		//char* temp = reinterpret_cast<char*>(realloc(_array, _malloc_size * sizeof(char)));
		char* temp = (char*)malloc(_malloc_size * sizeof(char));
		for (size_t i = 0; i < _now_free_index; ++i)
		{
			temp[i] = _array[i];
		}
		_array = temp;
	}
public:
	ArrayDoubleIncrease()
	{
		_malloc_size = 100;
		_now_free_index = 0;
		_array = reinterpret_cast<char*>(malloc(_malloc_size * sizeof(char)));
	}
	ArrayDoubleIncrease(size_t _s_)
	{
		_malloc_size = _s_;
		_now_free_index = 0;
		_array = reinterpret_cast<char*>(malloc(_malloc_size * sizeof(char)));
	}
	size_t GetSize() { return _now_free_index; }
	void InsertData(const char* _data_, size_t _s_)
	{
		size_t inserted_size = _s_ + _now_free_index;
		if (inserted_size >= _malloc_size)
		{
			DoubleSize();
		}
		for (size_t i = _now_free_index; i < inserted_size; ++i)
		{
			_array[i] = _data_[i];
		}
	}
	void InsertZeroIntoTail()
	{
		if (_now_free_index >= _malloc_size)
		{
			DoubleSize();
		}
		_array[_now_free_index] = '\0';
	}
	const char* GetArray() { return _array; }
	void ClearData() { _now_free_index = 0; }
	~ArrayDoubleIncrease()
	{
		free(_array);
	}
};
class CSimpleInfo : public CDialogEx
{
	DECLARE_DYNAMIC(CSimpleInfo)

public:
	CSimpleInfo(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CSimpleInfo();

	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SIMPLEINFO };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedSendfile();
public:
	CSerialPort mySerialPort;
	Timer refresh_timer;
	Timer reconnect_timer;
	size_t lasttime_bytes = 0;
	Timer count_timer;
private:
	ArrayDoubleIncrease<unsigned char> receive_data_origin;
	ArrayDoubleIncrease<char> receive_data_process;
public:
	ArrayDoubleIncrease<unsigned char>& GetOriginReceiveData() { return receive_data_origin; }
	ArrayDoubleIncrease<char>& GetProcessReceiveData() { return receive_data_process; }
public:
	// 数据源
	Queue<QueueCacheDataType*> _data_queue;
	size_t raw_data_ele_size = 1000;
	ProcessSoureDataFuncParam process_pic_func_param{ this,false };
	RefreshSimpleFuncParam refresh_pic_func_param{ this,false };
	// 数据源参数类
	DataSrcParam data_src_param;
	// 文件路径
	CString path;
	// tab
	CUart forms_cam_uart;
	CNetServer forms_cam_net_server;
	CNetClient forms_cam_net_client;
public:
	CComboBox m_combo_format;
	virtual BOOL OnInitDialog();
	CComboBox m_combo_coms;
	CComboBox m_combo_baudrate;
	CComboBox m_combo_databit;
	CComboBox m_combo_check;
	CComboBox m_combo_stopbit;
	CButton m_check_noerr;
	CButton m_check_reconnect;
	CButton m_check_tofile;
	volatile bool IS_STOP_RECONNECT = false;
	CEdit m_edit_receive;
	afx_msg void OnBnClickedButtonClearReceiveEditor();
	afx_msg void OnBnClickedButtonSend();
	afx_msg void OnSize(UINT nType, int cx, int cy);
private:
	// 改变窗体大小
	CRect rect_button;
	CRect rect_s_format;
	CRect rect_format;
	CRect rect_sendfile;
	CRect rect_path;
	CRect rect_clear;
	CRect rect_static_receive;
	CRect rect_static_send;
	CRect rect_receiveinfo;
	CRect rect_s_6;
	CRect rect_button_send;
public:
	CTabCtrl m_tab_mode;
	afx_msg void OnTcnSelchangeTabMode(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
