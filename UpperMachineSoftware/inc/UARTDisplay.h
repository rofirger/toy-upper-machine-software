#pragma once

#ifndef __AFXWIN_H__
#error "在包含此文件之前包含 'pch.h' 以生成 PCH"
#endif

#include "resource.h"		// 主符号
#include <Windows.h>
#include <cstring>
#include <exception>
#include <string>
#include "log.h"
class CUARTDisplayApp : public CWinApp
{
public:
	CUARTDisplayApp();

	// 重写
public:
	virtual BOOL InitInstance();

	// 实现

	DECLARE_MESSAGE_MAP()
	virtual CDocument* OpenDocumentFile(LPCTSTR lpszFileName);
};

extern CUARTDisplayApp theApp;

// 异常类
class MyException
{
public:
	MyException(const char* message)
		: message_(message)
	{}
	MyException(const MyException& other) : message_(other.message_)
	{}
	virtual ~MyException()
	{}

	const char* what() const
	{
		return message_.c_str();
	}
private:
	std::string message_;
};