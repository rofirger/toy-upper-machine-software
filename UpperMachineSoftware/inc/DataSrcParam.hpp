#ifndef _DATA_SRC_PARAM_HPP_
#define _DATA_SRC_PARAM_HPP_
#include <queue>
#include "queue_cache.h"
class DataSrcParam
{
private:
	// forms pointer
	void* _ptr_call_forms;
	// other pointers
	void* _ptr_data_pro_thread_func_param;
	void* _ptr_update_data_func_param;
	void* _ptr_clear_data_func_param;
	// function pointers
	void (*_ptr_func_enable_control)(void* _ptr_call_forms_, bool _b_);
	void (*_ptr_func_data_pro_thread)(void* _ptr_data_pro_thread_func_param_);
	void (*_ptr_func_update_data)(void* _ptr_update_data_func_param_);
	void (*_ptr_func_clear_data)(void* _ptr_clear_data_func_param_);
	// enable controls
	bool _b_control;
	// data cache
	Queue<QueueCacheDataType*>& _queue_data;
	size_t _num_data_queue_ele_default_size;
public:
	DataSrcParam(void* ptr_call_forms,
		void* ptr_data_pro_thread_func_param,
		void* ptr_update_data_func_param,
		void* ptr_clear_data_func_param,
		void (*ptr_func_enable_control)(void* ptr_call_forms_, bool b_),
		void (*ptr_func_data_pro_thread)(void* ptr_data_pro_thread_func_param_),
		void (*ptr_func_update_data_func)(void* ptr_update_data_func_param_),
		void (*ptr_func_clear_data_func)(void* ptr_clear_data_func_param_),
		bool b_control,
		Queue<QueueCacheDataType*>& queue_data, size_t num_data_queue_ele_default_size
	) :_queue_data(queue_data)
	{
		_ptr_call_forms = ptr_call_forms; // also can be regarded as  _ptr_func_enable_control_param
		_ptr_data_pro_thread_func_param = ptr_data_pro_thread_func_param;
		_ptr_update_data_func_param = ptr_update_data_func_param;
		_ptr_clear_data_func_param = ptr_clear_data_func_param;
		_ptr_func_enable_control = ptr_func_enable_control;
		_ptr_func_data_pro_thread = ptr_func_data_pro_thread;
		_ptr_func_update_data = ptr_func_update_data_func;
		_ptr_func_clear_data = ptr_func_clear_data_func;
		_b_control = b_control;
		_num_data_queue_ele_default_size = num_data_queue_ele_default_size;
	}
	~DataSrcParam()
	{
		while (!_queue_data.empty())
		{
			_queue_data.front()->Release();
			delete _queue_data.front();
			_queue_data.pop();
		}
	}
	void SetPtrFuncEnableControl(void(*ptr_func)(void* ptr, bool b)) { _ptr_func_enable_control = ptr_func; }
	void SetPtrFuncEnableControlParam(void* ptr_func_param) { _ptr_call_forms = ptr_func_param; }
	void SetPtrFuncDataProThread(void(*ptr_func)(void* ptr)) { _ptr_func_data_pro_thread = ptr_func; }
	void SetPtrFuncDataProThreadParam(void* ptr_func_param) { _ptr_data_pro_thread_func_param = ptr_func_param; }
	void SetPtrFuncUpdateData(void(*ptr_func)(void* ptr)) { _ptr_func_update_data = ptr_func; }
	void SetPtrFuncUpdateDataParam(void* ptr_func_param) { _ptr_update_data_func_param = ptr_func_param; }
	void SetPtrFuncClearData(void(*ptr_func)(void* ptr)) { _ptr_func_clear_data = ptr_func; }
	void SetPtrFuncClearDataParam(void* ptr_func_param) { _ptr_clear_data_func_param = ptr_func_param; }
	void SetControlEnable(bool b) { _b_control = b; }
	void SetDataQueueDefaultEleSize(size_t s) { _num_data_queue_ele_default_size = s; }
	void* GetPtrFuncEnableControl() { return _ptr_func_enable_control; }
	void* GetPtrFuncEnableControlParam() { return _ptr_call_forms; }
	void* GetPtrFuncDataProThread() { return _ptr_func_data_pro_thread; }
	void* GetPtrFuncDataProThreadParam() { return _ptr_data_pro_thread_func_param; }
	void* GetPtrFuncUpdateData() { return _ptr_func_update_data; }
	void* GetPtrFuncUpdateDataParam() { return _ptr_update_data_func_param; }
	void* GetPtrFuncClearData() { return _ptr_func_clear_data; }
	void* GetPtrFuncClearDataParam() { return _ptr_clear_data_func_param; }
	bool GetControlEnable() { return _b_control; }
	void* GetCallFormsPtr() { return _ptr_call_forms; }
	Queue<QueueCacheDataType*>& GetDataQueue() { return _queue_data; }
	size_t GetDataQueueDefaultEleSize() { return _num_data_queue_ele_default_size; }
};

#endif