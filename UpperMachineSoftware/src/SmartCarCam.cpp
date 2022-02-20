// SmartCarCam.cpp: 实现文件
//

#include "pch.h"
#include "UpperMachineSoftware.h"
#include "SmartCarCam.h"
#include "afxdialogex.h"

// SmartCarCam 对话框

IMPLEMENT_DYNAMIC(SmartCarCam, CDialogEx)

SmartCarCam::SmartCarCam(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SIMPLEINFO, pParent)
{
}

SmartCarCam::~SmartCarCam()
{
}

void SmartCarCam::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(SmartCarCam, CDialogEx)
END_MESSAGE_MAP()

// SmartCarCam 消息处理程序
