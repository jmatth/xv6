#include "types.h"
#include "defs.h"
#include "x86.h"
#include "mmu.h"
#include "param.h"
#include "proc.h"
#include "signal2.h"

void
sigrecieve(int sig, struct trapframe *tf)
{
  // Default actions
  if((int)proc->handlers[sig] == -1)
  {
    switch (sig) {
      case SIGSEGV:
        cprintf("pid %d %s: caught SIGSEGV, killed. "
                "eip 0x%x addr 0x%x\n",
                proc->pid, proc->name, tf->eip, rcr2());
        proc->killed = 1;
        break;
      default:
        return;
    }
  }

  // Push the current code location and appropriate handler location onto the
  // process's stack.
  // I'll make my own calling convention, with blackjack and hookers...
  PUSH(tf->eip);
  PUSH((uint)proc->handlers[sig]);

  // Have the process run the trampoline next time it is scheduled.
  tf->eip = (uint)proc->tramp;
}

