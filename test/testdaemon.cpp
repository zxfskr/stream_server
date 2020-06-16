/**
 * daemonize fork way
 *  1. fork a child thread, exit father thread
 *  2. setsid
 *  3. cd /
 *  4. stdout > /dev/null
 * 
 * @author zxf
 * @date 2020-04-03
 * @copyright GPL 2.0
 */

#include <iostream>

#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

void daemonize()
{
    pid_t pid;
    int fd;

    pid = fork();
    if (pid < 0)
    {
        std::cout << "cant create child suprocess!"
                  << std::endl;
    }
    else
    {
        if (pid != 0)
        {
            exit(0);
        }
    }

    setsid();

    if (chdir("/") < 0)
    {
        std::cout << "cant change directory!"
                  << std::endl;
        exit(-1);
    }

    fd = open("/dev/null", O_RDWR);
    dup2(fd, STDIN_FILENO);
    dup2(fd, STDOUT_FILENO);
    dup2(fd, STDERR_FILENO);
}

int main(int argc, char **argv)
{
    // daemonize();

    if (daemon(0, 0) == -1){
        std::cout << "error daemon!"
                  << std::endl;
        exit(-1);
    }

    while (1)
    {
        /* code */
        std::cout << "alive"
                  << std::endl;
        sleep(1);
    }

    return 0;
}