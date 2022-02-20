#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include "sock.h"
#pragma comment(lib, "ws2_32.lib")

MyCSock::MyCSock()
{
	WSADATA wsaData;
	WSAStartup(0x0202, &wsaData);
	m_hSocket = INVALID_SOCKET;
}

MyCSock::~MyCSock()
{
	Close();
}

BOOL MyCSock::Create(UINT nSocketPort, int nSockType, char* lpszSocketAddress)
{
	m_hSocket = socket(AF_INET, nSockType, 0);//创建套接字
	if (m_hSocket == INVALID_SOCKET)
		return false;
	//设置IP地址和端口
	sockaddr_in sa = { AF_INET };
	sa.sin_port = htons(nSocketPort);
	if (lpszSocketAddress)
		sa.sin_addr.s_addr = inet_addr(lpszSocketAddress);
	//绑定套接字和IP地址端口
	return !bind(m_hSocket, (sockaddr*)&sa, sizeof(sa));
}

int MyCSock::SendTo(const void* lpBuf, int nBufLen, UINT nHostPort, LPCSTR lpszHostAddress,
	int nFlags)
{
	sockaddr_in to = { AF_INET };
	to.sin_port = htons(nHostPort);
	to.sin_addr.s_addr = inet_addr(lpszHostAddress);
	return sendto(m_hSocket, (LPCSTR)lpBuf, nBufLen, nFlags, (sockaddr*)&to, sizeof(to));
}

int MyCSock::ReceiveFrom(void* lpBuf, int nBufLen, char* rSocketAddress, UINT& rSocketPort, int nFlags)
{
	sockaddr_in from = { AF_INET };
	int lenFrom = sizeof(from);
	int n = recvfrom(m_hSocket, (LPSTR)lpBuf, nBufLen, nFlags, (sockaddr*)&from, &lenFrom);
	strcpy(rSocketAddress, inet_ntoa(from.sin_addr));
	rSocketPort = htons(from.sin_port);
	return n;
}

BOOL MyCSock::GetPeerName(char* rSocketAddress, UINT& rSocketPort)
{
	sockaddr_in name = { AF_INET };
	int lenname = sizeof(name);
	if (getpeername(m_hSocket, (sockaddr*)&name, &lenname) < 0)
		return false;
	strcpy(rSocketAddress, inet_ntoa(name.sin_addr));
	rSocketPort = htons(name.sin_port);
	return true;
}

BOOL MyCSock::GetSockName(char* rSocketAddress, UINT& rSocketPort)
{
	sockaddr_in name = { AF_INET };
	int lenname = sizeof(name);
	if (getsockname(m_hSocket, (sockaddr*)&name, &lenname) < 0)
		return false;
	strcpy(rSocketAddress, inet_ntoa(name.sin_addr));
	rSocketPort = htons(name.sin_port);
	return true;
}

BOOL MyCSock::Accept(MyCSock& rConnectedSock, LPSTR szIp, UINT* nPort)
{
	sockaddr_in sa = { AF_INET };
	int nLen = sizeof(sa);
	rConnectedSock.m_hSocket = accept(this->m_hSocket, (sockaddr*)&sa, &nLen);
	if (rConnectedSock.m_hSocket == INVALID_SOCKET)
		return false;
	if (szIp)
		strcpy(szIp, inet_ntoa(sa.sin_addr));
	if (nPort)
		*nPort = htons(sa.sin_port);
	return true;
}

BOOL MyCSock::Connection(LPCSTR lpszHostAddress, UINT nPort)
{
	sockaddr_in sa = { AF_INET };
	sa.sin_port = htons(nPort);
	sa.sin_addr.s_addr = inet_addr(lpszHostAddress);
	return !connect(m_hSocket, (sockaddr*)&sa, sizeof(sa));
}
