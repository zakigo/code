#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>

// 测试端口和网络地址
#define _INT_PORT (8088)
#define _INT_BUF 1024

// udp 服务器主函数
int main(int argc, char* argv[]) {
    int sd, len;
    struct sockaddr_in addr = { AF_INET };
    socklen_t alen = sizeof addr;
    char msg[_INT_BUF];    
    
    //创建服务器socket 地址,客户端给它发送信息
    if((sd = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("main socket ");
        exit(sd);
    }
    // 这里简单输出连接信息
    printf("udp server start [%d][0.0.0.0][%d] -------> \n", sd, _INT_PORT);    

    //拼接对方地址
    addr.sin_port = htons(_INT_PORT);
    addr.sin_addr.s_addr = INADDR_ANY;
    if(bind(sd, (struct sockaddr*)&addr, sizeof addr) < 0){
        perror("main bind ");
        exit(-1);
    }
    
    // 循环处理消息读取发送到客户端
    while((len = recvfrom(sd, msg, sizeof msg - 1, 0, (struct sockaddr*)&addr, &alen))>0){
        msg[len] = '\0';
        printf("read [%s:%d] mag-->%s\n",inet_ntoa(addr.sin_addr), ntohs(addr.sin_port),  msg);
        //这里发送信息过去, 也可以事先connect这里就不绑定了
        sendto(sd, msg, len, 0, (struct sockaddr*)&addr, alen);
    }    

    close(sd);
    puts("udp server end ------------------------------<");

    return 0;
}
