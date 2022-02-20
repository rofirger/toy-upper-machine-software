// ChartCtrl.cpp: 实现文件
//

#include "pch.h"
#include "UpperMachineSoftware.h"
#include "ChartCtrl.h"
#include "afxdialogex.h"


// ChartCtrl 对话框

IMPLEMENT_DYNAMIC(ChartCtrl, CDialogEx)

ChartCtrl::ChartCtrl(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_PID, pParent)
{

}

ChartCtrl::~ChartCtrl()
{
}

void ChartCtrl::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(ChartCtrl, CDialogEx)
END_MESSAGE_MAP()


// ChartCtrl 消息处理程序
