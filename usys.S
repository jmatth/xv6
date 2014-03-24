#include "syscall.h"
#include "traps.h"

#define SYSCALL(name) \
  .globl name; \
  name: \
    movl $SYS_ ## name, %eax; \
    int $T_SYSCALL; \
    ret

SYSCALL(fork)
SYSCALL(exit)
SYSCALL(wait)
SYSCALL(pipe)
SYSCALL(read)
SYSCALL(write)
SYSCALL(close)
SYSCALL(kill)
SYSCALL(exec)
SYSCALL(open)
SYSCALL(mknod)
SYSCALL(unlink)
SYSCALL(fstat)
SYSCALL(link)
SYSCALL(mkdir)
SYSCALL(chdir)
SYSCALL(dup)
SYSCALL(getpid)
SYSCALL(sbrk)
SYSCALL(sleep)
SYSCALL(uptime)
SYSCALL(signal)

.globl __tramp
__tramp:
  # Normal callee stuff.
  pushl %ebp
  movl %esp, %ebp
  # Save all the registers.
  pushal

  # Call the handler. It is located right below our base pointer.
  call *4(%ebp)

  # Restore all the registers
  popal

  # Normal callee stuff again.
  movl %ebp, %esp
  popl %ebp

  # Since the handler address was between the base pointer and the return
  # address, we need to skip over it.
  addl $4, %esp

  # All done.
  ret

