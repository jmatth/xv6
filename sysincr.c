int sys_incr(void)
{
    static int counter = 0;
    return counter++;
}
