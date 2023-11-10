#pragma once
#include"TaskQueue.h"

class ThreadPool
{
public:
	/**
	* 创建线程池并初始化.
	* \param min			最小线程数
	* \param max			最大线程数
	*/
	ThreadPool(int min, int max);

	/**
	 * 销毁线程池.
	 */
	~ThreadPool();

	/**
	 * 往线程池添加任务.
	 */
	void threadPoolAdd(Task task);

	/**
	 * 获取线程池中忙的线程个数.
	 */
	int threadPoolBusyNum();

	/**
	 * 获取线程池中存活线程个数.
	 */
	int threadPoolLiveNum();


private:
	TaskQueue* m_taskQ;

	//管理者
	pthread_t managerID;		//管理者线程ID
	//工作者
	pthread_t* threadIDs;		//工作者线程ID
	int minNum;					//最小线程数
	int maxNum;					//最大线程数
	int busyNum;				//忙（工作）线程数
	int liveNum;				//存活线程数
	int exitNum;				//要销毁的线程数

	//确保线程安全
	pthread_mutex_t m_mutex;	//锁busyNum变量

	//阻塞生产者，消费者
	pthread_cond_t notEmpty;	//任务队列是否空了

	int shutdown = 0;				//是否要销毁线程池，销毁为1，不销毁为0


	/**
	* 工作者/消费者调用任务队列中的任务.
	*/
	// 为什么要用静态函数，类方法只有在实例化对象后才会有地址
	//	pthread_create第三个参数为函数指针的地址，所以定义为静态函数或者类外函数都可以
	static void* worker(void* arg);
	/**
	 * 管理者管理工作者线程.
	 */
	static void* manager(void* arg);

	/**
	 * 线程退出的时候将线程id置为0.
	 */
	void threadExit();

};

