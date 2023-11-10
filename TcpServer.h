#pragma once
#include "TcpSocket.h"
#include "ThreadPool.h"
#include "TaskQueue.h"
#include <arpa/inet.h>

class TcpServer
{
public:
    TcpServer();
    ~TcpServer();
    int setListen(unsigned short port);
    int getListenfd();

private:
    int m_fd;	// ¼àÌýµÄÌ×½Ó×Ö

};

void acceptConn(void* arg);
void working(void* arg);

struct SockInfo
{
    TcpSocket* tcp;
    struct sockaddr_in addr;
};

struct PoolInfo
{
    TcpServer* s;
    ThreadPool* pool;
};

