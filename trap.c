#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "x86.h"
#include "traps.h"
#include "spinlock.h"
#include "signal2.h"
#include "buf.h"
#include "fs.h"

// Interrupt descriptor table (shared by all CPUs).
struct gatedesc idt[256];
extern uint vectors[];  // in vectors.S: array of 256 entry pointers
struct spinlock tickslock;
uint ticks;

void
tvinit(void)
{
  int i;

  for(i = 0; i < 256; i++)
    SETGATE(idt[i], 0, SEG_KCODE<<3, vectors[i], 0);
  SETGATE(idt[T_SYSCALL], 1, SEG_KCODE<<3, vectors[T_SYSCALL], DPL_USER);
  
  initlock(&tickslock, "time");
}

void
idtinit(void)
{
  lidt(idt, sizeof(idt));
}

// Helper to check for and deliver SIGALRM
inline void
check_alarm(struct trapframe *tf)
{
  if(proc->next_alarm != 0 && proc->next_alarm <= ticks)
  {
    proc->next_alarm = 0;
    sigrecieve(SIGALRM, tf, 0, 0);
  }
}

// Helper to check for and deliver SIGSEGV
inline void
mmap_pgflt(struct trapframe *tf, uint cr2)
{
  struct buf *b;
  uint addr, upaddr;

//  if(tf->err % 2 == 0)
  mprotect(proc->pgdir, PGROUNDDOWN(cr2), PROT_READ);

  addr = (uint)uva2ka(proc->pgdir, (char *)cr2);
  addr = PGROUNDDOWN(addr);
  upaddr = addr + PGSIZE;
  for(; addr < upaddr; addr += BSIZE) {
    while((b = bfindmmap((uchar*)addr, 1)) != 0) {
      if(b->data != b->mmap_dst)
      {
        b->data = b->mmap_dst;
        if((b->flags & B_VALID) == 0)
          iderw(b);
        else
          memmove(b->mmap_dst, b->buf, BSIZE);
        b->flags |= B_VALID;
        brelse(b);
      } else
      {
        if((b->flags & B_WRITE) == 0)
        {
          brelse(b);
          sigrecieve(SIGSEGV, tf, cr2, tf->err);
          return;
        }
        b->flags |= B_DIRTY;
        brelse(b);
        mprotect(proc->pgdir, PGROUNDDOWN(cr2), PROT_WRITE);
      }
    }
  }
}

//PAGEBREAK: 41
void
trap(struct trapframe *tf)
{
  if(tf->trapno == T_SYSCALL){
    if(proc->killed)
      exit();
    proc->tf = tf;
    syscall();
    if(proc->killed)
      exit();
    check_alarm(tf);
    return;
  }

  switch(tf->trapno){
  case T_IRQ0 + IRQ_TIMER:
    if(cpu->id == 0){
      acquire(&tickslock);
      ticks++;
      wakeup(&ticks);
      release(&tickslock);
    }
    lapiceoi();
    break;
  case T_IRQ0 + IRQ_IDE:
    ideintr();
    lapiceoi();
    break;
  case T_IRQ0 + IRQ_IDE+1:
    // Bochs generates spurious IDE1 interrupts.
    break;
  case T_IRQ0 + IRQ_KBD:
    kbdintr();
    lapiceoi();
    break;
  case T_IRQ0 + IRQ_COM1:
    uartintr();
    lapiceoi();
    break;
  case T_IRQ0 + 7:
  case T_IRQ0 + IRQ_SPURIOUS:
    cprintf("cpu%d: spurious interrupt at %x:%x\n",
            cpu->id, tf->cs, tf->eip);
    lapiceoi();
    break;

  case T_PGFLT:
    if(proc == 0 || (tf->cs&3) == 0){
      // In kernel, it must be our mistake.
      cprintf("pagefault in kernel from cpu %d eip %x (cr2=0x%x)\n",
              cpu->id, tf->eip, rcr2());
      panic("kernel pagefault");
    }
    // was it mmaped?
    if (checkprot(proc->pgdir, rcr2(), PROT_MMAP)) {
      mmap_pgflt(tf, rcr2());
    } else {
      sigrecieve(SIGSEGV, tf, rcr2(), tf->err);
    }

    break;

  //PAGEBREAK: 13
  default:
    if(proc == 0 || (tf->cs&3) == 0){
      // In kernel, it must be our mistake.
      cprintf("unexpected trap %d from cpu %d eip %x (cr2=0x%x)\n",
              tf->trapno, cpu->id, tf->eip, rcr2());
      panic("trap");
    }
    // In user space, assume process misbehaved.
    cprintf("pid %d %s: trap %d err %d on cpu %d "
            "eip 0x%x addr 0x%x--kill proc\n",
            proc->pid, proc->name, tf->trapno, tf->err, cpu->id, tf->eip, 
            rcr2());
    proc->killed = 1;
  }

  // Force process exit if it has been killed and is in user space.
  // (If it is still executing in the kernel, let it keep running 
  // until it gets to the regular system call return.)
  if(proc && proc->killed && (tf->cs&3) == DPL_USER)
    exit();

  // Force process to give up CPU on clock tick.
  // If interrupts were on while locks held, would need to check nlock.
  if(proc && proc->state == RUNNING && tf->trapno == T_IRQ0+IRQ_TIMER)
    yield();

  // Check if the process has been killed since we yielded
  if(proc && proc->killed && (tf->cs&3) == DPL_USER)
    exit();

  // See if there's an alarm to ring
  if((tf->cs&3) == DPL_USER && proc != 0)
    check_alarm(tf);
}
