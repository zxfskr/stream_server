#include <iostream>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <strings.h>
#include <string.h>
#include <sys/epoll.h>
#include <fcntl.h>

#include <unistd.h>

#define PORT 10080
#define MESSAGE_SIZE 1024
#define MAX_EVENTS 20
#define TIMEOUT 500

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
    int flags = 1;

    int epoll_fd = -1;
    struct epoll_event ev;
    struct epoll_event events[MAX_EVENTS];
    int event_num;

    char buf[MESSAGE_SIZE];

    struct sockaddr_in localaddr;
    struct sockaddr_in remoteaddr;

    socklen_t address_len;

    socket_fd = socket(PF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1)
    {
        test_cout("failed to create socket!");
        goto fail;
    }

    flags = fcntl(socket_fd, F_GETFL, 0);
    fcntl(socket_fd, F_SETFL, flags | O_NONBLOCK);

    ret = setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    if (ret == -1)
    {
        test_cout("failed to set socket options!");
        goto fail;
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
        goto fail;
    }

    ret = listen(socket_fd, backlog);

    if (ret == -1)
    {
        test_cout("failed to listen !");
        goto fail;
    }

    epoll_fd = epoll_create1(EPOLL_CLOEXEC);
    if (epoll_fd == -1)
    {
        test_cout("failed to create epoll");
        goto fail;
    }

    //默认水平触发必要处理事件
    ev.events = EPOLLIN;
    ev.data.fd = socket_fd;

    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, socket_fd, &ev);

    for (;;)
    {
        event_num = epoll_wait(epoll_fd, events,
                               MAX_EVENTS, TIMEOUT);
        for (int i = 0; i < event_num; i++)
        {
            if (events[i].data.fd == socket_fd)
            {
                test_cout("建立链接");
                accept_fd = accept(socket_fd, (struct sockaddr *)&remoteaddr, &address_len);
                flags = fcntl(accept_fd, F_GETFL, 0);
                fcntl(accept_fd, F_SETFL, flags | O_NONBLOCK);

                //边缘触发接收数据，非必要处理事件
                ev.events = EPOLLIN | EPOLLET;
                ev.data.fd = accept_fd;

                epoll_ctl(epoll_fd, EPOLL_CTL_ADD, accept_fd, &ev);
            }
            else if (events[i].events & EPOLLIN)
            {
                do{
                    bzero(buf, MESSAGE_SIZE);
                    ret = recv(events[i].data.fd, buf, MESSAGE_SIZE, 0);
                    if (ret == 0)
                    {
                        // 断开链接
                        test_cout("关闭链接");
                        close(events[i].data.fd);
                        break;
                    }

                    if (ret == MESSAGE_SIZE)
                    {
                        test_cout("缓冲区满");
                    }
                }while (ret < 0 && errno == EINTR);


                if (ret < 0){
                    switch (errno)
                    {
                    case EAGAIN:
                        /* code */
                        break; 
                    default:
                        break;
                    }
                }

                if (ret > 0){
                    test_cout(buf);
                    ret = send(events[i].data.fd, buf, strlen(buf), 0);
                    if (ret <= 0)
                    {
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

fail:
    if (socket_fd > 0)
    {
        close(socket_fd);
    }
    return -1;
}
