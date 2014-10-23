#include "user.h"

struct ucontext {
    void *uc_stack;

    int eip;
    int esp;
    int ebp;
};

typedef struct ucontext ucontext_t;

int get_eip(void);

void init_stack(ucontext_t *ucp, int argc, int *argv) {
    int *space = (int *)ucp->uc_stack;
    int i;

    ucp->esp = (int)space;
    for(i = 0; i < argc; i++) {
        space[i] = argv[i];
    }

    ucp->esp += i;
    ucp->ebp = ucp->esp;
}

int getcontext(ucontext_t *ucp) {
    *ucp = *((ucontext_t *)malloc(sizeof(ucontext_t)));
    asm("\t movl %%ebp,%0" : "=r"(ucp->ebp));
    asm("\t movl %%esp,%0" : "=r"(ucp->esp));
    ucp->eip = get_eip();
    return 0;
}

void makecontext(ucontext_t *ucp, void(*func)(void), int argc, int *argv) {
    ucp->eip = (int)func;

    // Set up our stack
    init_stack(ucp, argc, argv);
}

int setcontext(ucontext_t *ucp) {
    // Restore esp and ebp from old context
    asm("\t movl %0, %%esp" : : "r"(ucp->esp));
    asm("\t movl %0, %%ebp" : : "r"(ucp->ebp));

    // Ret to continue execution from where we were before
    asm("jmp %0" : :"r"(ucp->eip));

    return 1;
}

int swapcontext(ucontext_t *oucp, ucontext_t *ucp) {
    // Store our current esp, ebp
    asm("\t movl %%ebp,%0" : "=r"(oucp->ebp));
    asm("\t movl %%esp,%0" : "=r"(oucp->esp));

    // Restore esp and ebp from old context
    asm("\t movl %0, %%esp" : : "r"(ucp->esp));
    asm("\t movl %0, %%ebp" : : "r"(ucp->ebp));

    oucp->eip = get_eip() + 4;

    // Ret to continue execution from where we were before
    asm("jmp %0" : :"r"(ucp->eip));

    return 1;
}

//Helpers to get register vals

int get_eip() {
    __asm__("call _here\n\t"
            "_here: pop %eax");
    extern int eax_var asm("%eax");
    return eax_var;
}
