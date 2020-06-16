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

#define PORT 10086
#define MESSAGE_SIZE 1024

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

struct Error : public DataHeader
{
    Error()
    {
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

    remoteaddr.sin_family = AF_INET;
    remoteaddr.sin_port = htons(PORT);
    remoteaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    bzero(&(remoteaddr.sin_zero), 8);

    ret = connect(socket_fd, (struct sockaddr *)&remoteaddr, sizeof(struct sockaddr));
    if (ret < 0)
    {
        test_cout("failed to connect server!");
        exit(-1);
    }

    for (;;)
    {
        bzero(buf, MESSAGE_SIZE);
        // gets(buf);
        ret = scanf("%[^\n]%*c", buf);
        if (ret == 0)
        {
            printf("input num : %d\n", sNum);
            getchar();
        }

        if (strcmp(buf, "quit") == 0)
        {
            test_cout("退出");
            break;
        }

        if (strcmp(buf, "login") == 0)
        {
            struct Login login = {};

            strcpy(login.userName, "haha");
            strcpy(login.password, "heihei");

            ret = send(socket_fd, (const void *)&login, sizeof(login), 0);
            if (ret <= 0)
            {
                test_cout("Failed to send data");
                break;
            }
            LogoutResult lir = {};
            recv(socket_fd, (char *)&lir, sizeof(lir), 0);
            printf("recv login result : %d\n", lir.result);
        }
        else if (strcmp(buf, "logout") == 0)
        {
            struct Logout logout = {};

            ret = send(socket_fd, (const void *)&logout, sizeof(logout), 0);
            if (ret <= 0)
            {
                test_cout("Failed to send data");
                break;
            }
            LogoutResult olr = {};
            recv(socket_fd, (char *)&olr, sizeof(olr), 0);
            printf("recv logout result : %d\n", olr.result);
        }
        else
        {
            printf("error cmd\n");
        }

        fflush(NULL);
    }
    close(socket_fd);

    return 0;
}
