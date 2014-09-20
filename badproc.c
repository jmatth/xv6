#include "user.h"
#include "fcntl.h"

int main(int argc, char *argv[])
{
    int pid;
    int output;
    int curruptime;

    if ((pid = fork()) > 0)
        exit();

    while(1)
    {
        curruptime = uptime();
        output = open("badproc.out", O_CREATE | O_WRONLY);
        printf(output, "%d\n", curruptime);
        close(output);
        sleep(1000);
    }
}
