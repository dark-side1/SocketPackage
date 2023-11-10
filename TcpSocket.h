#pragma once
#include<iostream>
#include<string>

class TcpSocket
{
public:
    TcpSocket();
    TcpSocket(int socket);
    ~TcpSocket();
    int connectToHost(std::string ip, unsigned short port);
    int sendMsg(std::string msg);
    std::string recvMsg();
    int getListenfd();

private:
    int readn(char* buf, int size);
    int writen(const char* msg, int size);

private:
    int m_fd;	// 通信的套接字

};

