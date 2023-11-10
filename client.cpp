#include "TcpSocket.h"
#include <sys/types.h>    
#include <sys/stat.h>    
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

int main()
{
    //1������ͨ�ŵ��׽���
    TcpSocket tcp;

    //2�����ӷ�����ip��port
    int ret = tcp.connectToHost("192.168.48.101", 12345);
    if (ret == -1) {
        return -1;
    }

    //3��ͨ��
    int fd1 = open("english.txt", O_RDONLY);
    int length = 0;
    char tmp[100];
    memset(tmp, 0, sizeof(tmp));
	while ((length = read(fd1, tmp, sizeof(tmp))) > 0)
    {
        //��������
        tcp.sendMsg(std::string(tmp, length));
        std::cout << "send Msg:" << std::endl;
        std::cout << tmp << std::endl << std::endl << std::endl;

        memset(tmp, 0, sizeof(tmp));

        //��������
        usleep(300);
    }
    sleep(10);

    return 0;
}