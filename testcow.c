#include "types.h"
#include "stat.h"
#include "user.h"

int
main(int argc, char *argv[])
{
  int i;
  int pid;
  int pre;
  int post;

  pre = uptime();
  for (i = 0; i < 400; i++) {
    pid = fork();
    if (pid == 0)
      exit();
    if (pid > 0)
      continue;
    if (pid < 0)
      while(wait() >= 0);
  }
  post = uptime();
  while(wait() >= 0);

  printf(1, "400 fork() calls took %d\n", post-pre);

  pre = uptime();
  for (i = 0; i < 400; i++) {
    pid = cowfork();
    if (pid == 0)
      exit();
    if (pid > 0)
      continue;
    if (pid < 0)
      while(wait() >= 0);
  }
  post = uptime();
  while(wait() >= 0);
  printf(1, "400 cowfork() calls took %d\n", post-pre);

  exit();
}
