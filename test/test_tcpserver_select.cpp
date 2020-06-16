 #include <iostream>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <strings.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#define PORT 10080
#define MESSAGE_SIZE 1024
#define FD_SIZE 1024

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
    int flags;
    int max_fd = -1;
    int events = 0;
    pid_t pid = -1;

    fd_set fd_sets;
    int curpos = -1;

    int accept_fds[FD_SIZE] = {0};

    char buf[MESSAGE_SIZE];

    struct sockaddr_in localaddr;
    struct sockaddr_in remoteaddr;

    socket_fd = socket(PF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1)
    {
        test_cout("failed to create socket!");
        exit(-1);
    }

    flags = fcntl(socket_fd, F_GETFL, 0);
    fcntl(socket_fd, F_SETFL, flags | O_NONBLOCK);

    max_fd = socket_fd;
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
        FD_ZERO(&fd_sets);
        FD_SET(socket_fd, &fd_sets);

        // select获取到事件时 会改变fd_sets的值，所以FD_ISSET可以仅
        //拿到有事件的fd, 也因此每次循环都需要将所有链接的fd重新设置到fd_sets
        for (int i = 0; i < FD_SETSIZE; i++)
        {
            if (accept_fds[i] != 0)
            {
                if (accept_fds[i] > max_fd)
                {
                    max_fd = accept_fds[i];
                }
                FD_SET(accept_fds[i], &fd_sets);
            }
        }

        events = select(max_fd + 1, &fd_sets, NULL, NULL, NULL);

        if (events < 0)
        {
            test_cout("failed to use select!");
            // exit(-1);
            break;
        }
        else if (events == 0)
        {
            test_cout("time out");
            continue;
        }
        else if (events)
        {
            // 事件触发时select会吧fd_set里面没有事件的fd去掉，因此判断socket_fd是否在
            // 可以得知是否有新的连接
            if (FD_ISSET(socket_fd, &fd_sets))
            {
                int i = 0;
                for (; i < FD_SIZE; i++)
                {
                    if (accept_fds[i] == 0)
                    {
                        curpos = i;
                        break;
                    }
                }

                if (i == FD_SIZE)
                {
                    printf("the connection is full!\n");
                    continue;
                }

                accept_fd = accept(socket_fd, (struct sockaddr *)&remoteaddr, &address_len);
                flags = fcntl(accept_fd, F_GETFL, 0);
                fcntl(accept_fd, F_SETFL, flags | O_NONBLOCK);

                accept_fds[curpos] = accept_fd;

                if (accept_fd > max_fd)
                {
                    max_fd = accept_fd;
                }

                test_cout("链接开始");
            }

            for (int i = 0; i < FD_SIZE; i++)
            {
                // 事件触发时select会吧fd_set里面没有事件的fd去掉，因此判断accept_fds成员是否在
                // 可以得知是否有新的消息
                if (accept_fds[i] != -1 && FD_ISSET(accept_fds[i], &fd_sets))
                {
                    bzero(buf, MESSAGE_SIZE);
                    ret = recv(accept_fds[i], buf, MESSAGE_SIZE, 0);
                    if (ret == 0)
                    {
                        close(accept_fds[i]);
                        accept_fds[i] = 0;
                        // 断开链接
                        test_cout("断开链接");
                        break;
                    }

                    test_cout(buf);

                    ret = send(accept_fds[i], buf, strlen(buf), 0);
                    if (ret <= 0)
                    {
                        close(accept_fds[i]);
                        accept_fds[i] = 0;
                        test_cout("Failed to send data");
                        break;
                    }
                }
            }
        }
    }

    if (pid != 0)
    {
        close(socket_fd);
    }
    return 0;
}
