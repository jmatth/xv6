#include <stdlib.h>

struct ucontext_stack {
    int *ebp;
    int *esp;
};

struct ucontext {
    struct ucontext_stack *stack;
    void *uc_stack;
    void (*func)(void);
    int argc;
    int *args;
};

typedef struct ucontext ucontext_t;
typedef struct ucontext_stack ucstack_t;
typedef struct regcontext regcontext_t;

ucstack_t *init_stack(void *mem, int argc, int *argv) {
    ucstack_t *s = (ucstack_t *) malloc(sizeof(ucstack_t));
    int *space = (int *)mem;
    int i;

    s->esp = space;
    for(i = 0; i < argc; i++) {
        space[i] = argv[i];
    }

    s->esp += i;
    s->ebp = s->esp;

    return s;
}

int getcontext(ucontext_t *ucp) {
    *ucp = *((ucontext_t *)malloc(sizeof(ucontext_t)));
    ucp->uc_stack = NULL;
    ucp->stack = NULL;
    ucp->func = NULL;
    ucp->args = NULL;
    ucp->argc = -1;
}

void makecontext(ucontext_t *ucp, void(*func)(void), int argc, int *argv) {
    ucp->func = func;
    ucp->argc = argc;
    ucp->args = argv;

    // Set up our stack
    ucp->stack = init_stack(ucp->uc_stack, argc, argv);
}

int swapcontext(ucontext_t *oucp, ucontext_t *ucp) {
    oucp->stack->ebp = ;
    oucp->stack->esp = ;
}
