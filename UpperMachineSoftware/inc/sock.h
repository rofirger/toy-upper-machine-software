#pragma once
#include <WinSock2.h>
class MyCSock
{
protected:
	SOCKET m_hSocket;
public:
	BOOL GetPeerName(char* rSocketAddress, UINT& rSocketPort);
	BOOL GetSockName(char* rSocketAddress, UINT& rSocketPort);
	BOOL Create(UINT nSocketPort = 0, int nSockType = SOCK_STREAM,
		char* lpszSocketAddress = NULL);
	BOOL Accept(MyCSock& rConnectedSock, LPSTR szIp = NULL, UINT* nPort = NULL);
	BOOL Connection(LPCSTR lpszHostAddress, UINT nPort);
	BOOL Listen(int nConnectionBacklog = 5)
	{
		return !listen(m_hSocket, nConnectionBacklog);
	}
	int Send(const void* lpBuf, int nBufLen, int nFlags = 0)
	{
		return send(m_hSocket, (LPCSTR)lpBuf, nBufLen, nFlags);
	}
	int SendTo(const void* lpBuf, int nBufLen, UINT nHostPort, LPCSTR lpszHostAddress = NULL,
		int nFlags = 0);
	int Receive(void* lpBuf, int nBufLen, int nFlags = 0)
	{
		return recv(m_hSocket, (char*)lpBuf, nBufLen, nFlags);
	}
	int ReceiveFrom(void* lpBuf, int nBufLen, char* rSocketAddress, UINT& rSocketPort, int nFlags = 0);
	void Close()
	{
		closesocket(m_hSocket);
		m_hSocket = INVALID_SOCKET;
	}
	MyCSock();
	virtual ~MyCSock();
};
