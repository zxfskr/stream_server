#include <iostream>

#include <unistd.h>
#include <signal.h>

void sighandler(int sig)
{
    std::cout << "receive signal"
              << sig
              << std::endl;
}

int main(int argc, char **argv)
{
    // signal(SIGINT, sighandle);
    // signal(SIGQUIT, sighandle);
    // signal(SIGHUP, sighandle);
    
    struct sigaction act, oact;
    act.sa_handler = sighandler;
    sigfillset(&act.sa_mask);
    act.sa_flags = 0;

    sigaction(SIGINT, &act, &oact);
    pause();

    return 0;
}
