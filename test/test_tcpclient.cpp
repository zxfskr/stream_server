#include <iostream>

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 10080
#define MESSAGE_SIZE 1024

void test_cout(const char *msg)
{
    std::cout << msg << std::endl;
}

int main(int argc, char **argv)
{
    int socket_fd;
    int accept_fd;
    int on = 1;
    int ret = -1;
    int backlog = 10;

    char buf[MESSAGE_SIZE];

    struct sockaddr_in localaddr;
    struct sockaddr_in remoteaddr;
    int address_len;

    socket_fd = socket(PF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1){
        test_cout("failed to create socket!");
        exit(-1);
    }

    ret = setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    if (ret == -1){
        test_cout("failed to set socket options!");
        exit(-1);
    }

    remoteaddr.sin_family = AF_INET;
    remoteaddr.sin_port = htons(PORT);
    remoteaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    bzero(&(remoteaddr.sin_zero), 8);

    ret = connect(socket_fd, (struct sockaddr *)&remoteaddr, sizeof(struct sockaddr));
    if (ret < 0){
        test_cout("failed to connect server!");
        exit(-1);
    }

    for (;;) {
        bzero(buf, MESSAGE_SIZE);
        // gets(buf);
        scanf("%[^\n]%*c", buf);  

        if (strcmp(buf, "quit") == 0){
            test_cout("退出");
            break;
        }
        ret = send(socket_fd, buf, strlen(buf), 0);
        if (ret <= 0){
            test_cout("Failed to send data");
            break;
        }

        bzero(buf, MESSAGE_SIZE);
        ret = recv(socket_fd, buf, MESSAGE_SIZE, 0);
        if (ret == 0){
            // 断开链接
            test_cout("Failed to recv data");
            break;
        }

        test_cout(buf);
    }
    close(socket_fd);

    return 0;
}
