# Assemble 32 bit instructions.
.code32

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
