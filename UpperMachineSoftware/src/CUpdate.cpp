// CUpdate.cpp: 实现文件
//

#include "pch.h"
#include "UpperMachineSoftware.h"
#include "CUpdate.h"
#include "afxdialogex.h"


// CUpdate 对话框

IMPLEMENT_DYNAMIC(CUpdate, CDialogEx)

CUpdate::CUpdate(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_UPDATE, pParent)
{

}

CUpdate::~CUpdate()
{
}

void CUpdate::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CUpdate, CDialogEx)
END_MESSAGE_MAP()


// CUpdate 消息处理程序
