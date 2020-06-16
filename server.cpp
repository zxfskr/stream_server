
#include <iostream>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <strings.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

enum
{
    CMD_LOGIN,
    CMD_LOGIN_RESULT,
    CMD_LOGOUT,
    CMD_LOGOUT_RESULT,
    CMD_ERROR
};

struct DataHeader
{
    short dataLength;
    short cmd;
};

struct Error:public DataHeader{
    Error(){
        dataLength = sizeof(Error);
        cmd = CMD_ERROR;
    }
};

struct Login : public DataHeader
{
    Login()
    {
        dataLength = sizeof(Login);
        cmd = CMD_LOGIN;
    };

    char userName[32];
    char password[32];
};

struct LoginResult : public DataHeader
{
    int result;
    LoginResult()
    {
        dataLength = sizeof(LoginResult);
        cmd = CMD_LOGIN_RESULT;
        result = 1;
    };

};

struct Logout : public DataHeader
{
    Logout()
    {
        dataLength = sizeof(Logout);
        cmd = CMD_LOGOUT;
    };
};

struct LogoutResult : public DataHeader
{
    int result;
    LogoutResult()
    {
        dataLength = sizeof(LogoutResult);
        cmd = CMD_LOGOUT_RESULT;
        result = 0;
    };
};

void test_cout(const char *msg)
{
    std::cout << msg << std::endl;
}

int init_socket_fd()
{
    int socket_fd;
    int on = 1;
    int ret = -1;
    int backlog = 10;
    int flags;
    int max_fd = -1;
    int events = 0;
    pid_t pid = -1;

    fd_set fd_sets;
    int curpos = -1;

    const int PORT = 10086;

    struct sockaddr_in localaddr;
    struct sockaddr_in remoteaddr;

    socket_fd = socket(PF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1)
    {
        test_cout("failed to create socket!");
        exit(-1);
    }

    // flags = fcntl(socket_fd, F_GETFL, 0);
    // fcntl(socket_fd, F_SETFL, flags | O_NONBLOCK);

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
    return socket_fd;
}

int main(int argc, char **argv)
{
    struct sockaddr_in remoteaddr;
    struct DataHeader dh = {};
    size_t dhLen = sizeof(DataHeader);
    socklen_t address_len = sizeof(struct sockaddr);
    int socket_fd = init_socket_fd();
    int accept_fd;
    while (true)
    {
        fd_set rfds;
        fd_set wfds;
        fd_set efds;

        select(socket_fd + 1, &rfds, &wfds, &efds, NULL);

        accept_fd = accept(socket_fd, (struct sockaddr *)&remoteaddr, &address_len);
        printf("链接开始： 接入 ip %s， port %d\n", inet_ntoa(remoteaddr.sin_addr), remoteaddr.sin_port);
        while (true)
        {
            char buf[1024];
            int nlen = recv(accept_fd, (char *)&buf, dhLen, 0);
            if (nlen == 0)
            {
                close(accept_fd);
                printf("链接断开\n");
                break;
            }

            memcpy(&dh, buf, dhLen);

            switch (dh.cmd)
            {
            case CMD_LOGIN:
            {
                Login login = {};
                recv(accept_fd, (char *)&login + dhLen, login.dataLength - dhLen, 0);
                printf("login name :%s pwd: %s\n", login.userName, login.password);

                LoginResult lir = {};

                send(accept_fd, (const void *)&lir, sizeof(lir), 0);
            }

            case CMD_LOGOUT:
            {
                Logout logout = {};
                recv(accept_fd, (char *)&logout + dhLen, logout.dataLength - dhLen, 0);
                printf("logout\n");

                LogoutResult lor = {};

                send(accept_fd, (const void *)&lor, sizeof(lor), 0);
            }
            default:
            {
                Error err = {};
                send(accept_fd, (const void *)&err, sizeof(err), 0);
            }
            }
        }
    }

    return 0;
}
