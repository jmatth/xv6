#include "types.h"
#include "defs.h"

int sys_incr(void)
{
    static int counter = 0;
    int param;

    argint(0, &param);

    if (param)
    {
        return (counter = 0);
    }

    return counter++;
}
