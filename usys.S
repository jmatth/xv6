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
SYSCALL(alarm)
SYSCALL(mprotect)
SYSCALL(dsbrk)

.globl __tramp
__tramp:
  # Normal callee stuff.
  pushl %ebp
  movl %esp, %ebp
  # Save all the registers.
  pushal
  # Copy the arguments to the top of the stack
  # Holy confusing syntax Batman!
  movl 12(, %ebp, 1), %ebx
  pushl %ebx
  movl 8(, %ebp, 1), %ebx
  pushl %ebx

  # Call the handler.
  call *4(%ebp)

  # Remove the arguments from the stack
  addl $8, %esp

  # Restore all the registers
  popal

  # Normal callee stuff again.
  movl %ebp, %esp
  popl %ebp

  # Since the handler address was between the base pointer and the return
  # address, we need to skip over it, along with the arguments
  addl $12, %esp

  # All done.
  ret

.globl signal
signal:
  # Inject the address of the trampoline into the arguments for sys_signal.
  popl %eax
  pushl $__tramp
  pushl %eax

  movl $SYS_signal, %eax
  int $T_SYSCALL

  # Remove the address of the trampoline when returning.
  ret $4
