#include <windows.h>
#include "uart.h"
#include <process.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <atlstr.h>
#include <malloc.h>
#include <list>

using  namespace std;
extern bool IS_GOOD_PID;
//extern double* _y;
extern double* _x;
extern size_t y_now_index;
extern size_t y_now_size;
extern std::list<double*> m_line_serie;
/** �߳��˳���־ */
bool CSerialPort::s_bExit = false;
/** ������������ʱ,sleep���´β�ѯ�����ʱ��,��λ:�� */
const UINT SLEEP_TIME_INTERVAL = 5;

CSerialPort::CSerialPort(void)
	: m_hListenThread(INVALID_HANDLE_VALUE)
{
	m_hComm = INVALID_HANDLE_VALUE;
	m_hListenThread = INVALID_HANDLE_VALUE;

	InitializeCriticalSection(&m_csCommunicationSync);
}

CSerialPort::~CSerialPort(void)
{
	CloseListenTread();
	ClosePort();
	DeleteCriticalSection(&m_csCommunicationSync);
}

bool CSerialPort::InitPort(UINT portNo /*= 1*/, UINT baud /*= CBR_9600*/, char parity /*= 'N'*/,
	UINT databits /*= 8*/, UINT stopsbits /*= 1*/, DWORD dwCommEvents /*= EV_RXCHAR*/)
{
	/** ��ʱ����,���ƶ�����ת��Ϊ�ַ�����ʽ,�Թ���DCB�ṹ */
	char szDCBparam[50];
	sprintf_s(szDCBparam, "baud=%d parity=%c data=%d stop=%d", baud, parity, databits, stopsbits);

	/** ��ָ������,�ú����ڲ��Ѿ����ٽ�������,�����벻Ҫ�ӱ��� */
	if (!openPort(portNo))
	{
		return false;
	}

	/** �����ٽ�� */
	EnterCriticalSection(&m_csCommunicationSync);

	/** �Ƿ��д����� */
	BOOL bIsSuccess = TRUE;

	/** �ڴ˿���������������Ļ�������С,���������,��ϵͳ������Ĭ��ֵ.
	*  �Լ����û�������Сʱ,Ҫע�������Դ�һЩ,���⻺�������
	*/
	/*if (bIsSuccess )
	{
	bIsSuccess = SetupComm(m_hComm,10,10);
	}*/

	/** ���ô��ڵĳ�ʱʱ��,����Ϊ0,��ʾ��ʹ�ó�ʱ���� */
	COMMTIMEOUTS  CommTimeouts;
	CommTimeouts.ReadIntervalTimeout = 0;
	CommTimeouts.ReadTotalTimeoutMultiplier = 0;
	CommTimeouts.ReadTotalTimeoutConstant = 0;
	CommTimeouts.WriteTotalTimeoutMultiplier = 0;
	CommTimeouts.WriteTotalTimeoutConstant = 0;
	if (bIsSuccess)
	{
		bIsSuccess = SetCommTimeouts(m_hComm, &CommTimeouts);
	}

	DCB  dcb;
	if (bIsSuccess)
	{
		// ��ANSI�ַ���ת��ΪUNICODE�ַ���
		DWORD dwNum = MultiByteToWideChar(CP_ACP, 0, szDCBparam, -1, NULL, 0);
		wchar_t* pwText = new wchar_t[dwNum];
		if (!MultiByteToWideChar(CP_ACP, 0, szDCBparam, -1, pwText, dwNum))
		{
			bIsSuccess = TRUE;
		}

		/** ��ȡ��ǰ�������ò���,���ҹ��촮��DCB���� */
		bIsSuccess = GetCommState(m_hComm, &dcb) && BuildCommDCB(pwText, &dcb);
		/** ����RTS flow���� */
		dcb.fRtsControl = RTS_CONTROL_ENABLE;

		/** �ͷ��ڴ�ռ� */
		delete[] pwText;
	}

	if (bIsSuccess)
	{
		/** ʹ��DCB�������ô���״̬ */
		bIsSuccess = SetCommState(m_hComm, &dcb);
	}

	/**  ��մ��ڻ����� */
	PurgeComm(m_hComm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);

	/** �뿪�ٽ�� */
	LeaveCriticalSection(&m_csCommunicationSync);

	return bIsSuccess == TRUE;
}

bool CSerialPort::InitPort(UINT portNo, const LPDCB& plDCB)
{
	/** ��ָ������,�ú����ڲ��Ѿ����ٽ�������,�����벻Ҫ�ӱ��� */
	if (!openPort(portNo))
	{
		return false;
	}

	/** �����ٽ�� */
	EnterCriticalSection(&m_csCommunicationSync);

	/** ���ô��ڲ��� */
	if (!SetCommState(m_hComm, plDCB))
	{
		return false;
	}

	/**  ��մ��ڻ����� */
	PurgeComm(m_hComm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);

	/** �뿪�ٽ�� */
	LeaveCriticalSection(&m_csCommunicationSync);

	return true;
}

void CSerialPort::ClosePort()
{
	/** ����д��ڱ��򿪣��ر��� */
	if (m_hComm != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hComm);
		m_hComm = INVALID_HANDLE_VALUE;
	}
}

bool CSerialPort::openPort(UINT portNo)
{
	/** �����ٽ�� */
	EnterCriticalSection(&m_csCommunicationSync);

	/** �Ѵ��ڵı��ת��Ϊ�豸�� */
	char szPort[50];
	sprintf_s(szPort, "COM%d", portNo);

	/** ��ָ���Ĵ��� */
	m_hComm = CreateFileA(szPort,  /** �豸��,COM1,COM2�� */
		GENERIC_READ | GENERIC_WRITE, /** ����ģʽ,��ͬʱ��д */
		0,                            /** ����ģʽ,0��ʾ������ */
		NULL,                         /** ��ȫ������,һ��ʹ��NULL */
		OPEN_EXISTING,                /** �ò�����ʾ�豸�������,���򴴽�ʧ�� */
		0,
		0);

	/** �����ʧ�ܣ��ͷ���Դ������ */
	if (m_hComm == INVALID_HANDLE_VALUE)
	{
		LeaveCriticalSection(&m_csCommunicationSync);
		return false;
	}

	/** �˳��ٽ��� */
	LeaveCriticalSection(&m_csCommunicationSync);

	return true;
}

bool CSerialPort::OpenListenThread()
{
	/** ����߳��Ƿ��Ѿ������� */
	if (m_hListenThread != INVALID_HANDLE_VALUE)
	{
		/** �߳��Ѿ����� */
		return false;
	}

	s_bExit = false;
	/** �߳�ID */
	UINT threadId;
	/** �����������ݼ����߳� */
	m_hListenThread = (HANDLE)_beginthreadex(NULL, 0, ListenThread, this, 0, &threadId);
	if (!m_hListenThread)
	{
		return false;
	}
	/** �����̵߳����ȼ�,������ͨ�߳� */
	if (!SetThreadPriority(m_hListenThread, THREAD_PRIORITY_ABOVE_NORMAL))
	{
		return false;
	}

	return true;
}

bool CSerialPort::CloseListenTread()
{
	if (m_hListenThread != INVALID_HANDLE_VALUE)
	{
		/** ֪ͨ�߳��˳� */
		s_bExit = true;

		/** �ȴ��߳��˳� */
		Sleep(10);

		/** ���߳̾����Ч */
		CloseHandle(m_hListenThread);
		m_hListenThread = INVALID_HANDLE_VALUE;
	}
	return true;
}

UINT CSerialPort::GetBytesInCOM()
{
	DWORD dwError = 0;  /** ������ */
	COMSTAT  comstat;   /** COMSTAT�ṹ��,��¼ͨ���豸��״̬��Ϣ */
	memset(&comstat, 0, sizeof(COMSTAT));

	UINT BytesInQue = 0;
	/** �ڵ���ReadFile��WriteFile֮ǰ,ͨ�������������ǰ�����Ĵ����־ */
	if (ClearCommError(m_hComm, &dwError, &comstat))
	{
		BytesInQue = comstat.cbInQue; /** ��ȡ�����뻺�����е��ֽ��� */
	}

	return BytesInQue;
}
UINT WINAPI CSerialPort::ListenThread(void* pParam)
{
	vector<unsigned char>vec;
	string pid_data;

	/** �õ������ָ�� */
	CSerialPort* pSerialPort = reinterpret_cast<CSerialPort*>(pParam);
	// �߳�ѭ��,��ѯ��ʽ��ȡ��������
	while (!pSerialPort->s_bExit)
	{
		UINT BytesInQue = pSerialPort->GetBytesInCOM();
		/** ����������뻺������������,����Ϣһ���ٲ�ѯ */
		if (BytesInQue == 0)
		{
			Sleep(SLEEP_TIME_INTERVAL);
			continue;
		}
		/** ��ȡ���뻺�����е����ݲ������ʾ */
		char cRecved = 0x00;
		int count = 0;

		do
		{
			cRecved = 0x00;
			switch (pSerialPort->GetReponseType())
			{
			case PID:
			{
				const char split_char[] = "\t";
				if (!IS_GOOD_PID && pSerialPort->ReadChar(cRecved) == true)
				{
					if (cRecved == '\n')
					{
						if (!pid_data.empty())
						{
							char* temp_str = NULL;
							char* pid_data_str = new char[pid_data.length() + 1];
							for (int i = 0; i <= pid_data.length(); ++i)
							{
								pid_data_str[i] = pid_data[i];
							}
							if (m_line_serie.size() == 0)
							{
								temp_str = strtok(pid_data_str, split_char);
								while (temp_str != NULL)
								{
									double* temp_double_serie = new double;
									temp_double_serie[y_now_index] = atof(temp_str);
									m_line_serie.push_back(temp_double_serie);
									temp_str = strtok(NULL, split_char);
								}
								y_now_index++;
							}
							else
							{
								auto iter_ = m_line_serie.begin();
								temp_str = strtok(pid_data_str, split_char);
								while (temp_str != NULL)
								{
									(*iter_)[y_now_index] = atof(temp_str);
									temp_str = strtok(NULL, split_char);
									iter_++;
								}
								y_now_index++;
							}
							delete[]pid_data_str;
							if (y_now_size - y_now_index <= 1)
							{
								y_now_size *= 2;
								for (auto plist = m_line_serie.begin(); plist != m_line_serie.end(); ++plist)
								{
									double* temp;
									temp = (double*)realloc(*plist, y_now_size * sizeof(double));
									if (temp == NULL)
									{
										::MessageBox(NULL, _T("�ڴ治��!"), _T("��ʾ"), 0);
										_endthreadex(0);
									}
									*plist = temp;
								}

								double* temp_x;
								temp_x = (double*)realloc(_x, y_now_size * sizeof(double));
								if (temp_x == NULL)
								{
									::MessageBox(NULL, _T("�ڴ治��!"), _T("��ʾ"), 0);
									_endthreadex(0);
								}
								_x = temp_x;
								for (int i = 0; i < y_now_size; ++i)
								{
									_x[i] = i + 1;
								}
							}

							// _y[y_now_index++] = atof(pid_data.c_str());
							pid_data.clear();
						}
						continue;
					}
					pid_data += cRecved;
				}
				break;
			}
			default:
				break;
			}
		} while (--BytesInQue);
	}

	return 0;
}

bool CSerialPort::ReadChar(char& cRecved)
{
	BOOL  bResult = TRUE;
	DWORD BytesRead = 0;
	if (m_hComm == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	/** �ٽ������� */
	EnterCriticalSection(&m_csCommunicationSync);

	/** �ӻ�������ȡһ���ֽڵ����� */
	bResult = ReadFile(m_hComm, &cRecved, 1, &BytesRead, NULL);
	if ((!bResult))
	{
		/** ��ȡ������,���Ը��ݸô�����������ԭ�� */
		DWORD dwError = GetLastError();

		/** ��մ��ڻ����� */
		PurgeComm(m_hComm, PURGE_RXCLEAR | PURGE_RXABORT);
		LeaveCriticalSection(&m_csCommunicationSync);

		return false;
	}

	/** �뿪�ٽ��� */
	LeaveCriticalSection(&m_csCommunicationSync);

	return (BytesRead == 1);
}
size_t CSerialPort::ReadChars(unsigned char* buff, size_t num_to_read)
{
	BOOL  bResult = TRUE;
	DWORD BytesRead = 0;
	if (m_hComm == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	/** �ٽ������� */
	EnterCriticalSection(&m_csCommunicationSync);

	/** �ӻ�������ȡһ���ֽڵ����� */
	bResult = ReadFile(m_hComm, buff, num_to_read, &BytesRead, NULL);
	if ((!bResult))
	{
		/** ��ȡ������,���Ը��ݸô�����������ԭ�� */
		DWORD dwError = GetLastError();

		/** ��մ��ڻ����� */
		PurgeComm(m_hComm, PURGE_RXCLEAR | PURGE_RXABORT);
		LeaveCriticalSection(&m_csCommunicationSync);

		return false;
	}

	/** �뿪�ٽ��� */
	LeaveCriticalSection(&m_csCommunicationSync);

	return BytesRead;
}
void CSerialPort::ReadData(unsigned char* buff)
{
	// �߳�ѭ��,��ѯ��ʽ��ȡ��������
	while (!this->Readexit)
	{
		UINT BytesInQue = this->GetBytesInCOM();
		/** ����������뻺������������,����Ϣһ���ٲ�ѯ */
		if (BytesInQue == 0)
		{
			Sleep(SLEEP_TIME_INTERVAL);
			continue;
		}

		memset(buff, 0x00, sizeof(buff));
		/** ��ȡ���뻺�����е����ݲ������ʾ */
		char cRecved = 0x00;
		int count = 0;
		do
		{
			cRecved = 0x00;
			if (this->ReadChar(cRecved) == true)
			{
				if (Readexit)
				{
					break;
				}
				//std::cout << UcharToHex(cRecved) << "\t";
				//out << (unsigned char)cRecved << ',';
				continue;
			}
		} while (--BytesInQue);
	}
}

bool CSerialPort::WriteData(unsigned char* pData, unsigned int length)
{
	BOOL   bResult = TRUE;
	DWORD  BytesToSend = 0;
	if (m_hComm == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	/** �ٽ������� */
	EnterCriticalSection(&m_csCommunicationSync);

	/** �򻺳���д��ָ���������� */
	bResult = WriteFile(m_hComm, pData, length, &BytesToSend, NULL);
	if (!bResult)
	{
		DWORD dwError = GetLastError();
		/** ��մ��ڻ����� */
		PurgeComm(m_hComm, PURGE_RXCLEAR | PURGE_RXABORT);
		LeaveCriticalSection(&m_csCommunicationSync);

		return false;
	}

	/** �뿪�ٽ��� */
	LeaveCriticalSection(&m_csCommunicationSync);

	return true;
}

void CSerialPort::Restart()
{
	CloseListenTread();
	ClosePort();
}

void CSerialPort::SetReponseType(ReponseType param)
{
	response_type = param;
}

ReponseType CSerialPort::GetReponseType()
{
	return response_type;
}

void CSerialPort::SetSimpleinfoPtr(CSimpleInfo* param)
{
	ptr_simpleinfo = param;
}

CSimpleInfo* CSerialPort::GetSimpleinfoPtr()
{
	return ptr_simpleinfo;
}

void CSerialPort::SetSimpleinfoFormat(SimpleinfoFormat param)
{
	simpleinfo_format = param;
}

SimpleinfoFormat CSerialPort::GetSimpleinfoFormat()
{
	return simpleinfo_format;
}