#pragma once
#include "sock.h"
#include "queue_cache.h"
#include "DataSrcParam.hpp"
#include <iostream>
#include <queue>
#include <string>
#include<functional>
unsigned __stdcall DataAcquisitionNetClient(LPVOID lpParam);
unsigned __stdcall ProcessFuncNetClient(LPVOID lpParam);
typedef struct NetClientDataAcParam
{
	Queue<QueueCacheDataType*>* ptr_queue_data;
	size_t data_queue_ele_default_size;
	MyCSock* ptr_sock;
	std::string ip;
	void* forms;
	int port;
}NetClientDataAcParam;

//using RefreshFuncParam = NetProcessFuncParam;
class CNetClient : public CDialogEx
{
	DECLARE_DYNAMIC(CNetClient)

public:
	CNetClient(DataSrcParam& data_src_param);
	virtual ~CNetClient();

	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_NET_CLIENT };
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
	NetClientDataAcParam data_ac_param;
	NetProcessFuncParam process_func_param{ this,_data_src_param.GetPtrFuncDataProThreadParam() };

public:
	CRect rect_static_boarder;
	CRect rect_static_ip;
	CRect rect_ipaddress;
	CRect rect_static_port;
	CRect rect_edit_port;
	CRect rect_button_main;
public:
	MyCSock sock;
	CEdit m_port;
	int port = 0;
	std::string ip;
	afx_msg void OnBnClickedMainButton();
	afx_msg void OnEnKillfocusEditPort();
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
