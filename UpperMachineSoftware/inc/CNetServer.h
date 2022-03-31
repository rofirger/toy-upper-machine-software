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
	CNetServer(DataSrcParam& data_src_param);
	virtual ~CNetServer();

	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_NET_SERVER };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	DataSrcParam& _data_src_param;
	Queue<QueueCacheDataType*>& data_queue_cache;
	size_t data_queue_cache_ele_default_size;
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
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
