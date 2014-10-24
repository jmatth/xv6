#include "user.h"
#include "context.h"

int get_eip(void);

void init_stack(ucontext_t *ucp, int argc, int *argv) {
    void *space = (int *)(ucp->uc_stack.ss_sp);
    int i;

    ucp->esp = space + ucp->uc_stack.ss_size - 1;
    for(i = 1; i <= argc; i++) {
        ucp->esp -= 4;
        *(int*)(ucp->esp) = argv[argc - i];
    }

    ucp->ebp = ucp->esp;
}

int getcontext(ucontext_t *ucp) {
    *ucp = *((ucontext_t *)malloc(sizeof(ucontext_t)));
    asm("\t movl (%%ebp),%0"   : "=r"(ucp->ebp));
    asm("\t movl -4(%%ebp),%0" : "=r"(ucp->eip));
    asm("\t movl %%ebp,%0"     : "=r"(ucp->esp));
    ucp->esp -=4;
    return 0;
}

void makecontext(ucontext_t *ucp, void(*func)(void), int argc, ...) {
    ucp->eip = func;
    int *argv = (int*) &argc + sizeof(int);

    // Set up our stack
    init_stack(ucp, argc, argv);
}

int setcontext(ucontext_t *ucp) {
    register uint target;
    target = (uint)ucp->eip;
    /* printf(1, "jumping to 0x%x\n", target); */
    /* printf(1, "ebp is 0x%x\n", ucp->ebp); */
    /* printf(1, "esp is 0x%x\n", ucp->esp); */
    // Restore esp and ebp from old context
    asm("\t movl %0, %%esp" : : "r"(ucp->esp));
    asm("\t movl %0, %%ebp" : : "r"(ucp->ebp));

    // Ret to continue execution from where we were before
    asm("jmp %0" : :"r"(target));

    return 1;
}

int swapcontext(ucontext_t *oucp, ucontext_t *ucp) {
    // Need to store eip for the jump in a reg
    register uint target;
    target = (uint)ucp->eip;

    // Store our callers esp, ebp, eip
    asm("\t movl (%%ebp),%0"   : "=r"(oucp->ebp));
    asm("\t movl -4(%%ebp),%0" : "=r"(oucp->eip));
    asm("\t movl %%ebp,%0"     : "=r"(oucp->esp));
    oucp->esp -=4;

    // Restore esp and ebp from old context
    asm("\t movl %0, %%esp" : : "r"(ucp->esp));
    asm("\t movl %0, %%ebp" : : "r"(ucp->ebp));

    // Ret to continue execution from where we were before
    asm("jmp %0" : :"r"(target));

    return 1;
}

//Helpers to get register vals

int get_eip() {
    __asm__("call _here\n\t"
            "_here: pop %eax");
    extern int eax_var asm("%eax");
    return eax_var;
}
