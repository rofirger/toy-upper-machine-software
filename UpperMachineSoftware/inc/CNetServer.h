#pragma once
#include "queue_cache.h"
#include "sock.h"
#include "DataSrcParam.hpp"
#include <iostream>
#include <queue>
unsigned __stdcall DataAcquisitionNetServer(LPVOID lpParam);
unsigned __stdcall ProcessFuncNetServer(LPVOID lpParam);
typedef struct NetServerDataAcParam
{
	Queue<QueueCacheDataType*>* ptr_queue_data;
	size_t data_queue_ele_default_size;
	MyCSock* ptr_sock;
	std::string ip;
	void* forms;
	int port;
}NetServerDataAcParam;
//using RefreshFuncParam = NetProcessFuncParam;
class CNetServer : public CDialogEx
{
	DECLARE_DYNAMIC(CNetServer)

public:
	//CNetServer(void* forms_csmc,
	//	void(*enable_control_csmc)(void* ptr_param_cscc, bool true_or_false),
	//	std::queue<QueueCacheDataType>& data_queue, size_t data_queue_ele_default_size,
	//	void (*_data_pro_thread_func)(void*), void* _data_pro_thread_func_param,
	//	void(*_update_data_func)(void*), void* _update_data_funcc_param,
	//	void(*_clear_data_func)(void*), void* _clear_data_func_param);   // 标准构造函数
	CNetServer(DataSrcParam& data_src_param);
	virtual ~CNetServer();

	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CAM_NET_SERVER };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	/*unsigned (*Reflesh_Pic)(LPVOID) = nullptr;
	void* refresh_pic_func_param = nullptr;
	void(*EnableControl)(void* ptr_param_cscc, bool true_or_false) = nullptr;
	void* CSmartCam = nullptr;*/
	DataSrcParam& _data_src_param;
	Queue<QueueCacheDataType*>& data_queue_cache;
	size_t data_queue_cache_ele_default_size;
	//void (*DataProcessFunc)(void*) = nullptr;
	//void* data_process_func_param = nullptr;
	//void (*UpdateParentFormData)(void*) = nullptr;
	//void* update_parent_form_data_param = nullptr;
	//void (*ClearParentFormData)(void*) = nullptr;
	//void* clear_data_func_param = nullptr;
	HANDLE handle_client;
	HANDLE handle_data_process;
	NetServerDataAcParam data_ac_param;
	//RefreshFuncParam refresh_func_param{ this,refresh_pic_func_param };
	NetProcessFuncParam process_func_param{ this,_data_src_param.GetPtrFuncDataProThreadParam() };
public:
	CRect rect_static_boarder;
	CRect rect_static_ip;
	CRect rect_ipaddress_ip;
	CRect rect_static_port;
	CRect rect_static_max_contact;
	CRect rect_edit_port;
	CRect rect_edit_max_contact;
	CRect rect_button_main;
public:
	virtual BOOL OnInitDialog();
	CIPAddressCtrl m_ip;
	afx_msg void OnBnClickedMainButton();
	MyCSock sock;
	MyCSock socka;//客户端套接字对象
	//获取客户端信息
	char sIp[20];//客户端IP地址
	UINT nPort;//客户端端口
	std::string ip;
	CEdit m_port;
	int port = 80;
	int max_contact = 1;
	afx_msg void OnEnKillfocusEditPort();
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
