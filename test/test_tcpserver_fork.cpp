#include <iostream>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <strings.h>
#include <string.h>

#include <unistd.h>

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
    pid_t pid = -1;

    char buf[MESSAGE_SIZE];

    struct sockaddr_in localaddr;
    struct sockaddr_in remoteaddr;

    socket_fd = socket(PF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1)
    {
        test_cout("failed to create socket!");
        exit(-1);
    }

    ret = setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    if (ret == -1)
    {
        test_cout("failed to set socket options!");
        exit(-1);
    }

    localaddr.sin_family = AF_INET;
    localaddr.sin_port = htons(PORT);
    localaddr.sin_addr.s_addr = INADDR_ANY;
    bzero(&(localaddr.sin_zero), 8);

    ret = bind(socket_fd, (const struct sockaddr *)&localaddr,
               sizeof(struct sockaddr_in));
    if (ret == -1)
    {
        test_cout("failed to bind !");
        exit(-1);
    }

    ret = listen(socket_fd, backlog);

    if (ret == -1)
    {
        test_cout("failed to listen !");
        exit(-1);
    }

    socklen_t address_len = sizeof(struct sockaddr);
    for (;;)
    {
        accept_fd = accept(socket_fd, (struct sockaddr *)&remoteaddr, &address_len);
        
        test_cout("获取接入fd");
        pid = fork();
        if (pid == 0)
        {
            test_cout("链接开始");
            for (;;)
            {
                bzero(buf, MESSAGE_SIZE);
                ret = recv(accept_fd, buf, MESSAGE_SIZE, 0);
                if (ret == 0)
                {
                    // 断开链接
                    break;
                }

                test_cout(buf);

                ret = send(accept_fd, buf, strlen(buf), 0);
                if (ret <= 0)
                {
                    test_cout("Failed to send data");
                    break;
                }
            }
            close(accept_fd);
            test_cout("链接断开");
        }
    }

    if (pid != 0)
    {
        close(socket_fd);
    }
    return 0;
}
