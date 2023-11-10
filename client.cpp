#include "TcpSocket.h"
#include <sys/types.h>    
#include <sys/stat.h>    
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

int main()
{
    //1、创建通信的套接字
    TcpSocket tcp;

    //2、连接服务器ip、port
    int ret = tcp.connectToHost("192.168.48.101", 12345);
    if (ret == -1) {
        return -1;
    }

    //3、通信
    int fd1 = open("english.txt", O_RDONLY);
    int length = 0;
    char tmp[100];
    memset(tmp, 0, sizeof(tmp));
	while ((length = read(fd1, tmp, sizeof(tmp))) > 0)
    {
        //发送数据
        tcp.sendMsg(std::string(tmp, length));
        std::cout << "send Msg:" << std::endl;
        std::cout << tmp << std::endl << std::endl << std::endl;

        memset(tmp, 0, sizeof(tmp));

        //接收数据
        usleep(300);
    }
    sleep(10);

    return 0;
}