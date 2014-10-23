#ifndef CONTEXT_H
#define CONTEXT_H 1

struct ucontext {
    void *uc_stack;

    int eip;
    int esp;
    int ebp;
};

typedef struct ucontext ucontext_t;

int getcontext(ucontext_t *ucp);
void makecontext(ucontext_t *ucp, void(*func)(void), int argc, int *argv);
int setcontext(ucontext_t *ucp);
int swapcontext(ucontext_t *oucp, ucontext_t *ucp);

#endif
