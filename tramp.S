.globl __tramp
__tramp:
  popl %eax
  popl %ecx
  popl %edx
  popl %ebx
  popl %ebp
  popl %esi
  popl %edi
  movl %ebp, %esp
  popl %ebp
  ret
