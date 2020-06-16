#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <st.h>

#include "srs_core_test.hpp"

#define PORT 10080
#define MESSAGE_SIZE 1024

void init(){
    int ret = -1;
    // use linux epoll
    ret = st_set_eventsys(ST_EVENTSYS_ALT);
    if (ret == -1){
        printf("st_set_eventsys use linux epoll failed.\n");
        return;
    }

    ret = st_init();
    if (ret != 0) {
        printf("st_init failed,\n");
        return;
    }

}

void listen(){
    int ret = -1;
    int fd = -1;
    int accept_fd = -1;

    char buf[1024] = {0,};
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1){
        printf("create linux socket failed\n");
        return;
    }

    int reuse_socket = 1;
    ret = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse_socket, sizeof(int));
    if (ret == -1){
        printf("setsocket reuse-addr error");
        return;
    }
    sockaddr_in addr;
    sockaddr_in remoteaddr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    ret = bind(fd, (const sockaddr*)&addr, sizeof(sockaddr_in));
    if (ret == -1){
        printf("bind error");
        return;
    }

    ret = listen(fd, 10);
    if (ret == -1){
        printf("listen error\n");
        return;
    }
    socklen_t address_len = sizeof(struct sockaddr);
    for (;;)
    {
        accept_fd = accept(fd, (struct sockaddr *)&remoteaddr, &address_len);

        for (;;){
            bzero(buf, MESSAGE_SIZE);
            ret = recv(accept_fd, buf, MESSAGE_SIZE, 0);
            if (ret == 0)
            {
                // 断开链接
                break;
            }

            printf("%s\n", buf);

            ret = send(accept_fd, buf, strlen(buf), 0);
            if (ret <= 0)
            {
                printf("Failed to send data");
                break;
            }
        }
        
        close(accept_fd);
    }

}

void cycle(){
    int ret = -1;
	// TODO: canbe a api thread.
	st_thread_exit(NULL);
	return;
}