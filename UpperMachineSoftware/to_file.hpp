#pragma once
#include "MyQueue.h"
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <cstring>
#include <fstream>
#include <chrono>
namespace rofirger
{
	class ToFileEleType
	{
	protected:
		std::string _file_path;
		std::string _file_data;
		int _width, _height, _kind;
	public:
		ToFileEleType() {};
		ToFileEleType(const std::string file_path, const std::string file_data, const int width, const int height, const int kind)
		{
			_file_path = file_path;
			_file_data = file_data;
			_width = width;
			_height = height;
			_kind = kind;
		}
		int GetHeight() { return _height; }
		int GetWidth() { return _width; }
		int GetKind() { return _kind; }
		std::string GetFilePath()
		{
			return _file_path;
		}
		std::string& GetFileData()
		{
			return _file_data;
		}
		~ToFileEleType() {}
	};
	// �����ݱ������ļ�
	class ToFile
	{
	private:
		Queue<ToFileEleType> _file_data;
		mutable std::mutex _mutex;
		std::queue<std::thread> _t;
		std::fstream file_stream;
	private:
		std::string DatetimeToString(tm tm_in)
		{
			tm* tm_ = &tm_in;						  // ��time_t��ʽת��Ϊtm�ṹ��
			int year, month, day, hour, minute, second;// ����ʱ��ĸ���int��ʱ������
			year = tm_->tm_year + 1900;                // ��ʱ�������꣬����tm�ṹ��洢���Ǵ�1900�꿪ʼ��ʱ�䣬������ʱ����intΪtm_year����1900��
			month = tm_->tm_mon + 1;                   // ��ʱ�������£�����tm�ṹ����·ݴ洢��ΧΪ0-11��������ʱ����intΪtm_mon����1��
			day = tm_->tm_mday;
			hour = tm_->tm_hour;
			minute = tm_->tm_min;
			second = tm_->tm_sec;
			auto chrono_time_now = std::chrono::system_clock::now();
			auto chrono_msecs = std::chrono::duration_cast<std::chrono::milliseconds>(chrono_time_now.time_since_epoch()).count()
				- std::chrono::duration_cast<std::chrono::seconds>(chrono_time_now.time_since_epoch()).count() * 1000;
			char s[20];                                // ����������ʱ��char*������
			sprintf(s, "%04d_%02d_%02d_%02d_%02d_%02d_%03lld", year, month, day, hour, minute, second, chrono_msecs);// ��������ʱ����ϲ���
			std::string str(s);                       // ����string����������������ʱ��char*������Ϊ���캯���Ĳ������롣
			return std::move(str);                    // ����ת������ʱ����string������
		}
	public:
		ToFile() {}
		static void WriteImgDataToFile(void* ptr_)
		{
			ToFile* p_user = reinterpret_cast<ToFile*>(ptr_);
			std::unique_lock<std::mutex> lock_(p_user->_mutex);
			while (!p_user->_file_data.empty())
			{
				p_user->file_stream.open(p_user->_file_data.front().GetFilePath(), std::ios::out | std::ios::binary);
				std::string& temp = p_user->_file_data.front().GetFileData();
				int width = p_user->_file_data.front().GetWidth();
				int height = p_user->_file_data.front().GetHeight();
				int kind = p_user->_file_data.front().GetKind();
				p_user->file_stream.write((char*)temp.c_str(), temp.length());
				p_user->file_stream.write((char*)&width, sizeof(int));
				p_user->file_stream.write((char*)&height, sizeof(int));
				p_user->file_stream.write((char*)&kind, sizeof(int));
				p_user->file_stream.close();
				p_user->_file_data.pop();
			}
		}
		void InsertImgFile(const std::string file_dic_path, const std::string  data, const int width, const int height, const int kind)
		{
			time_t timep;
			struct tm* param_time;
			time(&timep);
			param_time = localtime(&timep);
			std::string temp_time = DatetimeToString(*param_time);
			std::string filename_param = file_dic_path + "\\" + temp_time + ".sfp";
			_file_data.push(ToFileEleType(filename_param, data, width, height, kind));
			_t.push(std::thread(WriteImgDataToFile, this));
			while (!_t.front().joinable())
			{
				_t.pop();
			}
		}
		~ToFile()
		{
			while (!_t.empty())
			{
				_t.front().join();
				_t.pop();
			}

		}
	};
}
