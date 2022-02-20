
// UpdateDlg.h: 头文件
//

#pragma once
#include<functional>
#include<chrono>
#include<thread>
#include<atomic>
#include<memory>
#include<mutex>
#include<condition_variable>
// 异步，同步，定时器
class Timer
{
public:
	Timer() :expired_(true), try_to_expire_(false) {};

	Timer(const Timer& t)
	{
		expired_ = t.expired_.load();
		try_to_expire_ = t.try_to_expire_.load();
	}
	~Timer()
	{
		Expire();
	}

	void StartTimer(int interval, std::function<void()> task)
	{
		if (expired_ == false)
		{
			return;
		}
		expired_ = false;
		std::thread([this, interval, task]()
			{
				while (!try_to_expire_)
				{
					std::this_thread::sleep_for(std::chrono::milliseconds(interval));
					task();
				}
				{
					std::lock_guard<std::mutex> locker(mutex_);
					expired_ = true;
					expired_cond_.notify_one();
				}
			}).detach();
	}

	void Expire()
	{
		if (expired_)
		{
			return;
		}

		if (try_to_expire_)
		{
			return;
		}
		try_to_expire_ = true;
		{
			std::unique_lock<std::mutex> locker(mutex_);
			expired_cond_.wait(locker, [this] {return expired_ == true; });
			if (expired_ == true)
			{
				try_to_expire_ = false;
			}
		}
	}

	template<typename callable, class... arguments>
	void SyncWait(int after, callable&& f, arguments&&... args)
	{
		std::function<typename std::result_of<callable(arguments...)>::type()> task
		(std::bind(std::forward<callable>(f), std::forward<arguments>(args)...));
		std::this_thread::sleep_for(std::chrono::milliseconds(after));
		task();
	}
	template<typename callable, class... arguments>
	void AsyncWait(int after, callable&& f, arguments&&... args)
	{
		std::function<typename std::result_of<callable(arguments...)>::type()> task
		(std::bind(std::forward<callable>(f), std::forward<arguments>(args)...));

		std::thread([after, task]()
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(after));
				task();
			}).detach();
	}

private:
	std::atomic<bool> expired_;
	std::atomic<bool> try_to_expire_;
	std::mutex mutex_;
	std::condition_variable expired_cond_;
};

// CUpdateDlg 对话框
class CUpdateDlg : public CDialogEx
{
	// 构造
public:
	CUpdateDlg(CWnd* pParent = nullptr);	// 标准构造函数
	~CUpdateDlg();
	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_UPDATE_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

// 实现
protected:
	HICON m_hIcon;
	Timer _temer;
	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CProgressCtrl m_progress;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
class CBindCallback : public IBindStatusCallback
{
private:
	//CUpdate* m_pdlg;
public:
	CBindCallback();
	virtual ~CBindCallback();

	//IBindStatusCallback的方法。除了OnProgress     外的其他方法都返回E_NOTIMPL

	STDMETHOD(OnStartBinding)
		(DWORD dwReserved,
			IBinding __RPC_FAR* pib)
	{
		return E_NOTIMPL;
	}

	STDMETHOD(GetPriority)
		(LONG __RPC_FAR* pnPriority)
	{
		return E_NOTIMPL;
	}

	STDMETHOD(OnLowResource)
		(DWORD reserved)
	{
		return E_NOTIMPL;
	}

	//OnProgress在这里
	STDMETHOD(OnProgress)
		(ULONG ulProgress,
			ULONG ulProgressMax,
			ULONG ulStatusCode,
			LPCWSTR wszStatusText);

	STDMETHOD(OnStopBinding)
		(HRESULT hresult,
			LPCWSTR szError)
	{
		return E_NOTIMPL;
	}

	STDMETHOD(GetBindInfo)
		(DWORD __RPC_FAR* grfBINDF,
			BINDINFO __RPC_FAR* pbindinfo)
	{
		return E_NOTIMPL;
	}

	STDMETHOD(OnDataAvailable)
		(DWORD grfBSCF,
			DWORD dwSize,
			FORMATETC __RPC_FAR* pformatetc,
			STGMEDIUM __RPC_FAR* pstgmed)
	{
		return E_NOTIMPL;
	}

	STDMETHOD(OnObjectAvailable)
		(REFIID riid,
			IUnknown __RPC_FAR* punk)
	{
		return E_NOTIMPL;
	}

	// IUnknown方法.IE 不会调用这些方法的

	STDMETHOD_(ULONG, AddRef)()
	{
		return 0;
	}

	STDMETHOD_(ULONG, Release)()
	{
		return 0;
	}

	STDMETHOD(QueryInterface)
		(REFIID riid,
			void __RPC_FAR* __RPC_FAR* ppvObject)
	{
		return E_NOTIMPL;
	}
};

