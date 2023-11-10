#pragma once
#include"TaskQueue.h"

class ThreadPool
{
public:
	/**
	* �����̳߳ز���ʼ��.
	* \param min			��С�߳���
	* \param max			����߳���
	*/
	ThreadPool(int min, int max);

	/**
	 * �����̳߳�.
	 */
	~ThreadPool();

	/**
	 * ���̳߳��������.
	 */
	void threadPoolAdd(Task task);

	/**
	 * ��ȡ�̳߳���æ���̸߳���.
	 */
	int threadPoolBusyNum();

	/**
	 * ��ȡ�̳߳��д���̸߳���.
	 */
	int threadPoolLiveNum();


private:
	TaskQueue* m_taskQ;

	//������
	pthread_t managerID;		//�������߳�ID
	//������
	pthread_t* threadIDs;		//�������߳�ID
	int minNum;					//��С�߳���
	int maxNum;					//����߳���
	int busyNum;				//æ���������߳���
	int liveNum;				//����߳���
	int exitNum;				//Ҫ���ٵ��߳���

	//ȷ���̰߳�ȫ
	pthread_mutex_t m_mutex;	//��busyNum����

	//���������ߣ�������
	pthread_cond_t notEmpty;	//��������Ƿ����

	int shutdown = 0;				//�Ƿ�Ҫ�����̳߳أ�����Ϊ1��������Ϊ0


	/**
	* ������/�����ߵ�����������е�����.
	*/
	// ΪʲôҪ�þ�̬�������෽��ֻ����ʵ���������Ż��е�ַ
	//	pthread_create����������Ϊ����ָ��ĵ�ַ�����Զ���Ϊ��̬�����������⺯��������
	static void* worker(void* arg);
	/**
	 * �����߹��������߳�.
	 */
	static void* manager(void* arg);

	/**
	 * �߳��˳���ʱ���߳�id��Ϊ0.
	 */
	void threadExit();

};

