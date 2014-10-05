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
  // Push the current code location and appropriate handler location onto the
  // process's stack.
  // I'll make my own calling convention, with blackjack and hookers...
  PUSH(tf->eip);
  PUSH((uint)proc->handlers[sig]);

  // Have the process run the trampoline next time it is scheduled.
  tf->eip = (uint)proc->tramp;
}

