#include "TcpServer.h"
#include <arpa/inet.h>
#include <unistd.h>

TcpServer::TcpServer()
{
    m_fd = socket(AF_INET, SOCK_STREAM, 0);
}

TcpServer::~TcpServer()
{
	if (m_fd > 0) {
		close(m_fd);
	}
}

int TcpServer::setListen(unsigned short port)
{
	struct sockaddr_in saddr;
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(port);
	saddr.sin_addr.s_addr = INADDR_ANY;
	int ret = bind(m_fd, (struct sockaddr*)&saddr, sizeof(saddr));
	if (ret == -1) {
		perror("bind");
		return -1;
	}
	std::cout << "套接字绑定成功，ip：" << inet_ntoa(saddr.sin_addr) << ",port:" << port << std::endl;
	ret = listen(m_fd, 128);
	if (ret == -1) {
		perror("listen");
		return -1;
	}
	std::cout << "server init ok, start to accept new connect..." << std::endl;

    return ret;
}

int TcpServer::getListenfd() {
	return m_fd;
}

void acceptConn(void* arg)
{
	PoolInfo* poolInfo = (PoolInfo*)arg;
	socklen_t addrlen = sizeof(struct sockaddr_in);
	while (true)
	{
		SockInfo* info = new SockInfo;
		int cfd = accept(poolInfo->s->getListenfd(), (struct sockaddr*)&info->addr, &addrlen);
		if (cfd == -1) {
			perror("accept");
			break ;
		}
		//这里也要创建在堆区，不然while循环结束就释放了
		info->tcp = new TcpSocket(cfd);
		std::cout << "accept one new connect..." << std::endl;
		poolInfo->pool->threadPoolAdd(Task(working, info));
	}
	close(poolInfo->s->getListenfd());
	//已经在线程池中释放
	//delete poolInfo;
}

void working(void* arg) {
	std::cout << "server communicate working" << std::endl;
	struct SockInfo* pinfo = static_cast<struct SockInfo*>(arg);
	//建立连接成功，打印客户端的ip、port
	char ip[32];
	std::cout << "客户端ip：" << inet_ntop(AF_INET, &pinfo->addr.sin_addr.s_addr, ip, sizeof(ip))
		<< ",端口：" << ntohs(pinfo->addr.sin_port) << std::endl;
	//通信
	while (true)
	{
		std::cout << "接收数据：...." << std::endl;
		std::string msg = pinfo->tcp->recvMsg();
		if (!msg.empty()) {
			std::cout << msg << std::endl << std::endl << std::endl;
		}
		else {
			break;
		}
	}
	close(pinfo->tcp->getListenfd());
	//delete pinfo;
}