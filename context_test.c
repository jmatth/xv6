#include "user.h"
#include "context.h"

struct ucontext otherc1, otherc2, mainc;

void padding(){
  return;
}

void other1(int num_calls) {
  int called = 1;
  printf(1, "num_calls is %d\n", num_calls);
  while (called <= num_calls) {
    printf(1, "in other1 %d\n", called);
    called++;
    swapcontext(&otherc1, &otherc2);
  }
  setcontext(&mainc);
}

void other2() {
  printf(1, "In other2\n");
  setcontext(&otherc1);
}

int main(int argc, char *argv[])
{
  int switched = 0;
  printf(1, "Starting main\n");

  otherc1.uc_stack.ss_sp = malloc(512);
  printf(1, "main: stack starts at 0x%x\n", otherc1.uc_stack.ss_sp + 512 - 1);
  otherc2.uc_stack.ss_sp = malloc(512);
  otherc1.uc_stack.ss_size = 512;
  otherc2.uc_stack.ss_size = 512;
  makecontext(&otherc1, (void(*)())&other1, 1, 5);
  makecontext(&otherc2, &other2, 0, 0);

  getcontext(&mainc);

  if (switched)
  {
    printf(1, "Back in main, exiting\n");
    exit();
  }

  switched = 1;
  printf(1, "Setting context to other1\n");
  setcontext(&otherc1);

  exit();
}
