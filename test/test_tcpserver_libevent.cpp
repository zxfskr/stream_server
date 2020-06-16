#include <iostream>

#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/thread.h>

#include <arpa/inet.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#define PORT 10080

void test_cout(const char *msg)
{
    std::cout << msg << std::endl;
}

void on_read_cb(struct bufferevent *bev, void *ctx){
    struct evbuffer *input = bufferevent_get_input(bev);
    struct evbuffer *output = bufferevent_get_output(bev);
    /* 把读入的数据全部复制到写内存中 */
    /* Copy all the data from the input buffer to the output buffer. */
    // evbuffer_add_buffer(output, input);
    evbuffer_add(output, "haha", strlen("haha"));
}

void on_accept_cb(struct evconnlistener *listener, evutil_socket_t fd, struct sockaddr* addr, int socklen, void *ctx)
{
    struct event_base *base = NULL;
    struct bufferevent *bev = NULL;
    base = evconnlistener_get_base(listener);
    bev = bufferevent_socket_new(base, fd, 0);

    bufferevent_enable(bev, EV_READ|EV_WRITE);

    bufferevent_setcb(bev, on_read_cb, NULL, NULL, NULL);
}

int main(int argc, char **argv)
{
    struct event_base *base = NULL;
    void *ctx = NULL;
    struct sockaddr_in serveraddr;
    struct evconnlistener *listener;

    base = event_base_new();
    if (!base)
    {
        puts("coudnt open event base!");
        return 1;
    }

    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = INADDR_ANY;
    serveraddr.sin_port = htons(PORT);
    listener = evconnlistener_new_bind(base,
                                       on_accept_cb,
                                       ctx,
                                       LEV_OPT_REUSEABLE,
                                       10,
                                       (struct sockaddr *)&serveraddr,
                                       sizeof(serveraddr));
    if (!listener){
        perror("coudnt create listener!");
        return 1;
    }

    // evconnlistener_set_error_cb(listener, accept_error_cb);
    event_base_dispatch(base);
    return 0;
}
