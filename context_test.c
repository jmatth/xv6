#include "user.h"
#include "context.h"

struct ucontext otherc1, otherc2;

void padding(){
  return;
}

void other1() {
  static int called = 0;
  printf(1, "In other1\n");
  if (called < 5) {
    called++;
    setcontext(&otherc2);
  } else {
    printf(1, "Exiting in other1\n");
    exit();
  }
}

void other2() {
  printf(1, "In other2\n");
  setcontext(&otherc1);
}

int main(int argc, char *argv[])
{
  printf(1, "Starting main\n");

  otherc1.uc_stack.ss_sp = malloc(512);
  printf(1, "main: stack starts at 0x%x\n", otherc1.uc_stack.ss_sp + 512 - 1);
  otherc2.uc_stack.ss_sp = malloc(512);
  otherc1.uc_stack.ss_size = 512;
  otherc2.uc_stack.ss_size = 512;
  makecontext(&otherc1, &other1, 0, 0);
  makecontext(&otherc2, &other2, 0, 0);

  printf(1, "Setting context to other1\n");
  setcontext(&otherc1);

  exit();
}
