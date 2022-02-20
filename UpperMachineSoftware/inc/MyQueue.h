#pragma once
#include<iostream>
using namespace std;
template<typename T>
class Queue
{
public:
	//�˴�����ͷ�ڵ�
	Queue() :_front(NULL), _rear(NULL), _count(0)
	{}

	~Queue()
	{
		clear();
	}

	void push(const T& node)
	{
		if (_front == NULL)
			_front = _rear = new QueueNode(node);
		else
		{
			QueueNode* newqueuenode = new QueueNode(node);
			_rear->next = newqueuenode;
			_rear = newqueuenode;
		}
		_count++;
	}

	bool empty() const
	{
		return _front == NULL;
	}


	int size() const
	{
		return _count;
	}


	void clear()
	{
		while (_front)
		{
			QueueNode* FrontofQueue = _front;
			_front = _front->next;
			delete FrontofQueue;
		}
		_count = 0;
	}


	void pop()
	{
		if (empty())
		{
			return;
		}
		QueueNode* FrontofQueue = _front;
		_front = _front->next;
		delete FrontofQueue;
		_count--;
	}


	T& front()
	{
		if (empty())
		{
			cerr << "Error, queue is empty!";
		}
		return _front->data;
	}
private:       //Ҳ����ֱ����������listֱ�ӹ���
	struct  QueueNode
	{
		T data;
		QueueNode* next;
		QueueNode(const T& Newdata, QueueNode* nextnode = NULL) :data(Newdata), next(nextnode)
		{ }
		// QueueNode() = default;
	};
	QueueNode* _front;  //��ͷָ��
	QueueNode* _rear;  // ��βָ��
	int _count;
};
