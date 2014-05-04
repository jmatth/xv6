#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "signal.h"

int
sys_fork(void)
{
  return fork();
}

int
sys_clone(void)
{
  void (*func)(void*);
  void *arg;
  void *stack;

  argint(0, (int*)&func);
  argint(1, (int*)&arg);
  argint(2, (int*)&stack);

  return clone(func, arg, stack);
}

int
sys_join(void)
{
  void **stack;
  argint(0, (int*)&stack);
  return join(stack);
}

int
sys_mutex_init(void)
{
  return mutex_init();
}

int
sys_mutex_trylock(void)
{
  int md;
  argint(0, &md);
  return mutex_trylock(md);
}

int
sys_mutex_lock(void)
{
  int md;
  argint(0, &md);
  return mutex_lock(md);
}

int
sys_mutex_unlock(void)
{
  int md;
  argint(0, &md);
  return mutex_unlock(md);
}

int
sys_mutex_destroy(void)
{
  int md;
  argint(0, &md);
  return mutex_destroy(md);
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

    if(proc->next_alarm != 0 && proc->next_alarm <= ticks)
      break;

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
  unsigned int msecs;
  int currtime;
  int old_alarm;

  argint(0, (int*)&msecs);
  old_alarm = proc->next_alarm;
  currtime = sys_uptime();

  if(msecs == 0)
    proc->next_alarm = 0;
  else
    proc->next_alarm = currtime + msecs;

  if(old_alarm > 0 && old_alarm > currtime)
    return old_alarm - currtime;

  return 0;
}
