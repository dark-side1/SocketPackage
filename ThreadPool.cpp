#include "ThreadPool.h"
#include <iostream>
#include <string.h>
#include <unistd.h>

const int NUMBER = 2;


ThreadPool::ThreadPool(int min, int max)
{
	do
	{
		//任务队列
		m_taskQ = new TaskQueue;
		//工作者
		threadIDs = new pthread_t[max];
		if (this->threadIDs == nullptr)
		{
			std::cout << "malloc threadIDs fail..." << std::endl;
			break;
		}
		memset(this->threadIDs, 0, sizeof(pthread_t) * max);
		this->minNum = min;					//最小线程数
		this->maxNum = max;					//最大线程数
		this->busyNum = 0;				//忙（工作）线程数
		this->liveNum = min;				//存活线程数
		this->exitNum = 0;				//要销毁的线程数
		//线程安全
		if (pthread_mutex_init(&m_mutex, nullptr) != 0 || pthread_cond_init(&notEmpty, nullptr) != 0) {
			std::cout << "mutex or cond fail..." << std::endl;
			break;
		}

		this->shutdown = 0;

		//创建线程
		pthread_create(&this->managerID, nullptr, manager, this);
		for (int i = 0; i < min; ++i) {
			pthread_create(&this->threadIDs[i], nullptr, worker, this);
		}

	} while(0);
}

ThreadPool::~ThreadPool()
{
	this->shutdown = 1;

	pthread_join(this->managerID, nullptr);

	for (int i = 0; i < this->liveNum; i++)
	{
		pthread_cond_signal(&this->notEmpty);
	}
	if (this->m_taskQ) delete this->m_taskQ;
	if (this->threadIDs) delete[]this->threadIDs;
	pthread_mutex_destroy(&this->m_mutex);
	pthread_cond_destroy(&this->notEmpty);
}

void ThreadPool::threadPoolAdd(Task task)
{
	if (this->shutdown) {
		return;
	}
	// 添加任务，不需要加锁，任务队列中有锁
	this->m_taskQ->addTask(task);

	pthread_cond_signal(&this->notEmpty);
}

int ThreadPool::threadPoolBusyNum()
{
	int busy_num;
	pthread_mutex_lock(&this->m_mutex);
	busy_num = this->busyNum;
	pthread_mutex_unlock(&this->m_mutex);
	return busy_num;
}

int ThreadPool::threadPoolLiveNum()
{
	int live_num;
	pthread_mutex_lock(&this->m_mutex);
	live_num = this->liveNum;
	pthread_mutex_unlock(&this->m_mutex);
	return live_num;
}

void* ThreadPool::worker(void* arg)
{
	ThreadPool* pool = static_cast<ThreadPool*>(arg);
	while (true)
	{
		pthread_mutex_lock(&pool->m_mutex);
		while (pool->m_taskQ->taskNumber() == 0 && !pool->shutdown)//为什么是while不是if
		//资源释放的时候从wait开始执行，要用while在判断一次，条件满足才能向下执行
		{
			pthread_cond_wait(&pool->notEmpty, &pool->m_mutex);

			if (pool->exitNum) {
				pool->exitNum--;
				if (pool->liveNum > pool->minNum) {
					pool->liveNum--;
					pthread_mutex_unlock(&pool->m_mutex);
					pool->threadExit();
				}
			}
		}
		
		if (pool->shutdown) {
			pthread_mutex_unlock(&pool->m_mutex);
			pool->threadExit();
		}

		Task task = pool->m_taskQ->takeTask();
		pool->busyNum++;
		pthread_mutex_unlock(&pool->m_mutex);

		//工作者线程执行任务
		std::cout << "thread " << std::to_string(pthread_self()) << " is start working" << std::endl;
		task.function(task.arg); //(*task.function)(task.arg);
		delete task.arg;
		task.arg = nullptr; 
		std::cout << "thread " << std::to_string(pthread_self()) << " is end working" << std::endl;
		pthread_mutex_lock(&pool->m_mutex);
		pool->busyNum--;
		pthread_mutex_unlock(&pool->m_mutex);
	}
	return nullptr;
}

void* ThreadPool::manager(void* arg)
{
	ThreadPool* pool = static_cast<ThreadPool*>(arg);
	while (!pool->shutdown)
	{
		sleep(3);

		pthread_mutex_lock(&pool->m_mutex);
		int task_num = pool->m_taskQ->taskNumber();
		int live_num = pool->liveNum;
		int busy_num = pool->busyNum;
		pthread_mutex_unlock(&pool->m_mutex);
		//创建线程
		//任务个数 > 存活的线程个数 && 存活的线程个数 < 最大线程数
		if (task_num > live_num - busy_num && live_num < pool->maxNum) {
			pthread_mutex_lock(&pool->m_mutex);
			int counter = 0;
			for (int i = 0; i < pool->maxNum && counter < NUMBER && pool->liveNum < pool->maxNum; i++)
			{
				if (pool->threadIDs[i] == 0) {
					pthread_create(&pool->threadIDs[i], nullptr, worker, pool);
					counter++;
					pool->liveNum++;
				}
			}
			pthread_mutex_unlock(&pool->m_mutex);
		}
		//销毁线程
		//忙线程数量*2 < 存活的线程个数 && 存活的线程个数 > 最小线程数
		if (busy_num * 2 < live_num && live_num > pool->minNum)
		{
			pthread_mutex_lock(&pool->m_mutex);
			pool->exitNum = NUMBER;
			pthread_mutex_unlock(&pool->m_mutex);
			for (int i = 0; i < NUMBER; i++)
			{
				pthread_cond_signal(&pool->notEmpty);
			}
		}

	}
	return nullptr;
}

void ThreadPool::threadExit()
{
	pthread_t tid = pthread_self();
	for (int i = 0; i < this->maxNum; i++)
	{
		if (this->threadIDs[i] == tid) {
			this->threadIDs[i] = 0;
			std::cout << "threadExit() is called, " << std::to_string(pthread_self()) << " exit" << std::endl;
			break;
		}
	}
	pthread_exit(nullptr);
}
