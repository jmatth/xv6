#ifndef CONTEXT_H
#define CONTEXT_H 1

struct uc_stack {
  void *ss_sp;
  uint ss_size;
};

struct ucontext {
    struct uc_stack uc_stack;

    void* eip;
    void* esp;
    void* ebp;
};

typedef struct ucontext ucontext_t;

int getcontext(ucontext_t *ucp);
void makecontext(ucontext_t *ucp, void(*func)(void), int argc, int, ...);
int setcontext(ucontext_t *ucp);
int swapcontext(ucontext_t *oucp, ucontext_t *ucp);

#endif
