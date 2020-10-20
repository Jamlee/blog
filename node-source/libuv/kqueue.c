///////////////////////////////////////////////////////
//
// 简单启动一个 kqueue, 用于理解异步的代码
//
///////////////////////////////////////////////////////

#include <sys/socket.h>
#include <sys/event.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#define MYPORT  8887
#define QUEUE   20
#define BUFFER_SIZE 1024

int main()
{
    // 定义sockfd AF_INET（IPv4） AF_INET6（IPv6） AF_LOCAL（UNIX协议） AF_ROUTE（路由套接字） AF_KEY（秘钥套接字）
    // SOCK_STREAM（字节流套接字） SOCK_DGRAM
    int server_sockfd = socket(AF_INET, SOCK_STREAM, 0);

    // 定义sockaddr_in
    struct sockaddr_in server_sockaddr;
    server_sockaddr.sin_family = AF_INET;
    server_sockaddr.sin_port = htons(MYPORT);
    server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    // bind，成功返回0，出错返回-1
    if(bind(server_sockfd,(struct sockaddr *)&server_sockaddr,sizeof(server_sockaddr))==-1)
    {
        perror("bind");
        exit(1);
    }

    printf("监听%d端口\n", MYPORT);
    // listen，成功返回0，出错返回-1
    if(listen(server_sockfd, QUEUE) == -1)
    {
        perror("listen");
        exit(1);
    }

    //创建一个消息队列并返回kqueue描述符
    int kq =  kqueue();
    struct kevent change_list;  //想要监控的事件
    struct kevent event_list[10000];  //用于kevent返回
    char buffer[1024];
    int nevents;
    
    // 监听sock的读事件
    EV_SET(&change_list, server_sockfd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, 0);
    while(1) {
        printf("new loop...\n");
        // 等待监听事件的发生
        nevents = kevent(kq, &change_list, 1, event_list, 2, NULL);
        if (nevents < 0) {
            printf("kevent error.\n");  // 监听出错
        } else if (nevents > 0) {
            printf("get events number: %d\n", nevents);
            for (int i = 0; i < nevents; ++i) {
                printf("loop index: %d\n", i);
                struct kevent event = event_list[i]; //监听事件的event数据结构
                int clientfd = (int) event.ident;  // 监听描述符
                // 表示该监听描述符出错
                if (event.flags & EV_ERROR) {
                    close(clientfd);
                    printf("EV_ERROR: %s\n", strerror(event_list[i].data));
                }
                // 表示sock有新的连接
                if (clientfd == server_sockfd) {
                    printf("new connection\n");
                    struct sockaddr_in client_addr;
                    socklen_t client_addr_len = sizeof(client_addr);
                    int new_fd = accept(server_sockfd, (struct sockaddr *) &client_addr, &client_addr_len);
                    long len = recv(new_fd, buffer, sizeof(buffer), 0);
                    char remote[INET_ADDRSTRLEN];
                    printf("connected with ip: %s, port: %d\n",
                           inet_ntop(AF_INET, &client_addr.sin_addr, remote, INET_ADDRSTRLEN),
                           ntohs(client_addr.sin_port));
                    send(new_fd, buffer, len, 0);
                }
            }
        }
    }
    return 0;
}