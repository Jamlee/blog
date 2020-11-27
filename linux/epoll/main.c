#include <stdio.h>

// 提供 socket 
#include <arpa/inet.h>

#include <sys/socket.h>
#include <sys/types.h>

#include <sys/epoll.h>


#define SERVER_PORT    (7778)
#define EPOLL_MAX_NUM  (2048) //最多2048个fd
#define BUFFER_MAX_LEN (4096)

char buffer[BUFFER_MAX_LEN];

int main() {
    int listen_fd = 0;
    int client_fd = 0;
    
    // 下方类型， 定义 socket 编程需要的结构, 来自 arpa/inet.h 中 引入的arpa/in.h
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    socklen_t client_len;

    int epfd = epoll_create(1000);
    return 0;
}
