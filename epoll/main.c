#include <stdio.h>
#include <sys/epoll.h>


#define SERVER_PORT    (7778)
#define EPOLL_MAX_NUM  (2048) //最多 2048 个  fd
#define BUFFER_MAX_LEN (4096)

char buffer[BUFFER_MAX_LEN];

int main() {
    int listen_fd = 0;
    int client_fd = 0;
    
    int epfd = epoll_create(1000);
    return 0;
}
