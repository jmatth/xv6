#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "signal.h"
#include "softtimer.h"
#include "timer.h"

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return proc->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = proc->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  int remaining;
  uint usecs;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(proc->killed){
      release(&tickslock);
      return -1;
    }

    if(proc->next_alarm != 0)
    {
      if(proc->next_alarm < ticks)
      {
        break;
      }
      else if(proc->next_alarm == ticks)
      {
        usecs = read_PIT_count();
        usecs = TIMER_FREQ/TIMER_IPS - usecs;
        if(proc->next_alarm_usecs <= usecs)
        {
          break;
        }
      }
    }

    sleep(&ticks, &tickslock);
  }
  remaining = n - (ticks - ticks0);
  release(&tickslock);

  if(remaining > 0)
    return remaining;
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;
  
  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

sighandler_t
sys_signal(void)
{
    int signo;
    sighandler_t tramp;
    sighandler_t handler;
    sighandler_t old;

    argint(0, (int*)&tramp);
    argint(1, &signo);
    argint(2, (int*)&handler);

    if (signo > NSIGS-1)
      return (sighandler_t)-1;

    proc->tramp = tramp;
    old = proc->handlers[signo];
    proc->handlers[signo] = handler;

    return old;
}

int
sys_alarm(void)
{
  uint usecs;
  uint tmsecs;
  uint currtime;
  uint old_alarm;

  argint(0, (int*)&usecs);
  old_alarm = proc->next_alarm;
  currtime = sys_uptime();

  if(usecs == 0)
    proc->next_alarm = 0;
  else
  {
    tmsecs = usecs / 10000;
    usecs = usecs % 10000;
    proc->next_alarm = currtime + tmsecs;
    proc->next_alarm_usecs = usecs;
  }

  if(old_alarm > 0 && old_alarm > currtime)
    return old_alarm - currtime;

  return 0;
}

ushort
sys_microuptime(void)
{
  return read_PIT_count();
}
