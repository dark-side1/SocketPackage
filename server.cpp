#include "TcpServer.h"
#include "TcpSocket.h"
#include <arpa/inet.h>
#include <iostream>
#include <unistd.h>

int main() {
	//一定要用指针，创建在堆区，不然直接main结束，线程池和TcpServer就析构了


	//创建监听套接字
	TcpServer* s = new TcpServer();
	//绑定本地ip、port并设置监听
	s->setListen(12345);
	//创建线程池
	ThreadPool* pool = new ThreadPool(3, 10);
	//要分配内存空间，不然会段错误
	//成员变量要用指针，因为ThreadPool、TcpServer没有复制构造函数
	PoolInfo* info = new PoolInfo;
	info->pool = pool;
	info->s = s;
	pool->threadPoolAdd(Task(acceptConn, info));

	pthread_exit(nullptr);
	return 0;
}

