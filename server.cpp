#include "TcpServer.h"
#include "TcpSocket.h"
#include <arpa/inet.h>
#include <iostream>
#include <unistd.h>

int main() {
	//һ��Ҫ��ָ�룬�����ڶ�������Ȼֱ��main�������̳߳غ�TcpServer��������


	//���������׽���
	TcpServer* s = new TcpServer();
	//�󶨱���ip��port�����ü���
	s->setListen(12345);
	//�����̳߳�
	ThreadPool* pool = new ThreadPool(3, 10);
	//Ҫ�����ڴ�ռ䣬��Ȼ��δ���
	//��Ա����Ҫ��ָ�룬��ΪThreadPool��TcpServerû�и��ƹ��캯��
	PoolInfo* info = new PoolInfo;
	info->pool = pool;
	info->s = s;
	pool->threadPoolAdd(Task(acceptConn, info));

	pthread_exit(nullptr);
	return 0;
}

