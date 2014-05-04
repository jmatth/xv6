#include "types.h"
#include "x86.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

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
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
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
