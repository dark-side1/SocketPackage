#pragma once
#include<queue>
#include<pthread.h>

using callback = void(*)(void*);

struct Task
{
	callback function;	//����ָ���Ǳ�������ջ��
	void* arg;			//ָ���ڶ�������Ҫ�ͷ�
	Task() {
		function = nullptr;
		arg = nullptr;
	}
	Task(callback func, void* arg) {
		this->function = func;
		this->arg = arg;
	}
};

class TaskQueue
{
public:
	TaskQueue();
	~TaskQueue();

	void addTask(Task &task);
	void addTask(callback func, void* arg);

	Task takeTask();

	inline size_t taskNumber() {
		return m_queue.size();//todo Ϊʲô������
	}


private:
	pthread_mutex_t m_mutex;
	std::queue<Task> m_queue;
};

