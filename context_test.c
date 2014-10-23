#include "user.h"
#include "context.h"

ucontext_t maincontext, othercontext;

void other() {
  printf(1, "In other, swapping to main...\n");
  setcontext(&maincontext);
}

int main(int argc, char *argv[])
{
  int count = 0;
  printf(1, "Starting main\n");
  getcontext(&maincontext);
  makecontext(&othercontext, &other, 0, 0);

  if (count < 10)
  {
    count++;
    printf(1, "Swapping to other...\n");
    setcontext(&othercontext);
  }
  else
  {
    exit();
  }

  exit();
}
