#pragma once
#include <iostream>
#include <queue>
#include "MyQueue.h"
// 请注意该类的成员堆区内存需要专门的Release()函数释放
class QueueCacheDataType
{
private:
	size_t _size_malloc;
	size_t _size;
	unsigned char* _array;
public:
	QueueCacheDataType()
	{
		_size_malloc = 0;
		_size = 0;
		_array = NULL;
	}
	QueueCacheDataType(size_t array_size) :_size(0), _size_malloc(array_size)
	{
		if (_size_malloc == 0)
		{
			_array = NULL;
			return;
		}
		_array = new unsigned char[_size_malloc];
	}
	QueueCacheDataType(const QueueCacheDataType& user_)
	{
		_size_malloc = user_._size_malloc;
		_size = user_._size;
		if (_size_malloc == 0)
		{
			_array = NULL;
			return;
		}
		unsigned char* temp = new unsigned char[_size_malloc];
		_array = temp;
		for (long long i = 0; i < _size_malloc; ++i)
		{
			_array[i] = user_._array[i];
		}
	}
	QueueCacheDataType& operator=(const QueueCacheDataType& user_)
	{
		if (this->_array != nullptr)
		{
			delete[]this->_array;
		}
		this->_size_malloc = user_._size_malloc;
		this->_size = user_._size;
		if (_size_malloc == 0)
		{
			_array = NULL;
			return *this;
		}
		_array = new unsigned char[_size_malloc];
		for (long long i = 0; i < _size_malloc; ++i)
		{
			this->_array[i] = user_._array[i];
		}
		return *this;
	}
	unsigned char* GetArray() { return _array; }
	size_t GetSize() { return _size; }
	void SetSize(size_t size_param) { _size = size_param; }
	void Release()
	{
		if (_array != NULL)
			delete[]_array;
		_array = NULL;
		_size_malloc = 0; _size = 0;
	}
	~QueueCacheDataType() { Release(); }
};
using PicData = QueueCacheDataType;

typedef enum ProcessPicStatus
{
	FINDIND_FIRST_HEAD_ELE,
	FINDIND_SECOND_HEAD_ELE,
	FINDING_FIRST_TAIL_ELE,
	FINDING_SECOND_TAIL_ELE
}ProcessPicStatus;

typedef struct ProcessSoureDataFuncParam
{
	void* form;
	bool is_stop;
}ProcessSoureDataFuncParam;
using RefreshPicFuncParam = ProcessSoureDataFuncParam;
using RefreshSimpleFuncParam = ProcessSoureDataFuncParam;

typedef struct
{
	void* forms;
	void* data_process_func_param;
}NetProcessFuncParam, UartProcessFuncParam;