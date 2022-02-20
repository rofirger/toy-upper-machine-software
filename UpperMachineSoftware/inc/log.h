// based on the C++11 standard
// the Log doesn't support wide character path. 
#ifndef _ROFIRGER_LOG_H_
#define _ROFIRGER_LOG_H_
#include <string>
#include <memory>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#define ROFIRGER_LOG_RELEASE 1
namespace rofirger
{
	typedef enum LOG_LEVEL
	{
		LOG_LEVEL_INFO = 0x00,
		LOG_LEVEL_WARNING = 0x01,
		LOG_LEVEL_ERROR = 0x02,
		LOG_LEVEL_FATAL = 0x03
	}LOG_LEVEL;
	typedef enum LogEnvironment
	{
		LOG_ENVIRONMENT_DEBUG = 0x00,
		LOG_ENVIRONMENT_RELEASE = 0x01
	}LogEnvironment;
#if defined(ROFIRGER_LOG_DEBUG)
#define add_log(_log_level_, ...) Log::GetInstance()->AddLog(_log_level_,rofirger::LOG_ENVIRONMENT_DEBUG,__FILE__,__LINE__,__FUNCSIG__,__VA_ARGS__)
#elif defined(ROFIRGER_LOG_RELEASE)
#define add_log(_log_level_, ...) Log::GetInstance()->AddLog(_log_level_,rofirger::LOG_ENVIRONMENT_RELEASE,__FILE__,__LINE__,__FUNCSIG__,__VA_ARGS__)
#endif
	class Log
	{
	public:
		static Log* GetInstance()noexcept;
		void SetMsgBufferSize(const size_t _s_)noexcept;
		void SetFolderPath(const char* _folder_path_)noexcept;
		void SetLogFileMaxSize(const long _s_)noexcept;
		bool StartLog()noexcept;
		void StopLog()noexcept;
		void AddLog(LOG_LEVEL _log_level_, LogEnvironment _log_environment_, const char* _file_, size_t _line_num_, const char* _func_sig_, const char* fmt_, ...)noexcept;
	private:
		Log() = default;
		~Log();
		Log(const Log& _log_) = delete;
		Log& operator=(const Log& _log_) = delete;
		void ThreadFunc();
		void CheckFileSize();
	private:
		const std::string _level_str[4]{ "INFO","WARNING","ERROR","FATAL" };
		static Log* _ptr_log;
		std::mutex _mutex;
		static std::mutex _mutex_get_instance;
		std::condition_variable _cv;
		std::string _folder_path;
		std::string _file_path;
		FILE* _ptr_file = nullptr;
		std::queue<std::string> _queue_log_data;
		std::shared_ptr<std::thread>    _thread;
		bool _is_stop;
		// default:1024, the size can be reassigned.
		size_t _msg_buffer_size = 1024;
		/* default: 10M, the size can be reassigned.
		 * The value indicates that log stream will new another log file when current log output file size is greater than this value.
		*/
		long _log_file_overflow_size = 10 * 1024 * 1024;
	};
}
#endif