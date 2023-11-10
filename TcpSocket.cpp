#include "TcpSocket.h"
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>

TcpSocket::TcpSocket()
{
	m_fd = socket(AF_INET, SOCK_STREAM, 0);
}

TcpSocket::TcpSocket(int socket)
{
	m_fd = socket;
}

TcpSocket::~TcpSocket()
{
	if (m_fd > 0)
	{
		close(m_fd);
	}
}

int TcpSocket::connectToHost(std::string ip, unsigned short port)
{
	//连接服务器IP、port
	struct sockaddr_in saddr;
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(port);
	inet_pton(AF_INET, ip.c_str(), &saddr.sin_addr.s_addr);
	int ret = connect(m_fd, (struct sockaddr*)&saddr, sizeof(saddr));
	if (ret == -1)
	{
		perror("connect");
		return -1;
	}
	std::cout << "成功和服务器建立连接..." << std::endl;
	return ret;
}

int TcpSocket::sendMsg(std::string msg)
{
	//申请内存空间：数据长度 + 包头长度（4字节，存储数据长度）
	char* data = new char[msg.size() + 4];
	int bigLen = htonl(msg.size());
	memcpy(data, &bigLen, 4);
	memcpy(data + 4, msg.c_str(), msg.size());
	//发送数据
	int ret = writen(data, msg.size() + 4);
	delete []data;
	return ret;
}

std::string TcpSocket::recvMsg()
{
	//接收数据
	//读数据头
	int len = 0;
	readn((char*)&len, 4);
	len = ntohl(len);
	std::cout << "数据块大小：" << len << std::endl;

	//根据读出的长度分配内存
	char* buf = new char[len + 1];
	int ret = readn(buf, len);
	if (ret != len) {
		return std::string();
	}
	buf[len] = '\0';
	std::string retStr(buf);
	delete[]buf;
	return retStr;
}

int TcpSocket::getListenfd()
{
	return m_fd;
}

int TcpSocket::readn(char* buf, int size)
{
	int remain_read = size;
	int nread = 0;
	char* p = buf;
	while (remain_read > 0) {
		if ((nread = read(m_fd, p, remain_read)) > 0) {
			p += nread;
			remain_read -= nread;
		}
		else if (nread == -1) {
			return -1;
		}
	}
	return size;
}

int TcpSocket::writen(const char* msg, int size)
{
	int remain_write = size;
	int nwrite = 0;
	const char* p = msg;
	while (remain_write > 0) {
		if ((nwrite = write(m_fd, msg, remain_write)) > 0) {
			p += nwrite;
			remain_write -= nwrite;
		}
		else if(nwrite == -1){
			return -1;
		}
	}
	return size;
}
