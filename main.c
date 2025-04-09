#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include"TcpServer.h"
typedef struct TcpServer  TcpServer;
int main(int argc, char* argv[])
{
    printf("开始运行...\n");
    if (argc < 3) {
        printf("./a.out port path fail...\n");
        return -1;
    }
    printf("正在启动服务器...\n");
    unsigned short port = atoi(argv[1]);
    //切换进程路径
    chdir(argv[2]);
    //TcpServer* initTcpServer(unsigned short port, int size);
    //初始化服务器实例
    TcpServer* tcpServer= initTcpServer(port,1);//线程池工作线程为1
    //启动服务器
    tcpServerRun(tcpServer);
    printf("serverRun...\n");
    return 0;
}