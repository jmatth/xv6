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

int
sys_mprotect(void)
{

  int addr, addr_align;
  int len;
  int prot;
  int res;

  argint(0, &addr);
  argint(1, &len);
  argint(2, &prot);

  // Sanitize prot
  prot = prot & (PROT_NONE | PROT_READ | PROT_WRITE);

  pte_t *pgdir = proc->pgdir;

  addr_align = PGROUNDDOWN(addr - 1);
  while(addr_align <= addr + len - 1)
  {
    res = mprotect(pgdir, (uint)addr_align, (uint)prot);
    if(res < 0) {
      return res;
    }
    addr_align += PGSIZE;
  }

  return res;
}
