#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include "shm.h"

int main()
{
  int var = 1;
  shm_t *shm = shm_new(&var, sizeof var);

  int pid;
  if ((pid = fork()) == 0)
  { /* child */
    var = 5;
    shm_write(shm, &var);
    printf("child: %d\n", var);
    return 0;
  }
  /* Wait for child to return */
  int status;
  while (wait(&status) != pid);
  /* */
  shm_read(&var, shm);
  /* Parent is updated by child */
  printf("parent: %d\n", var);
  shm_del(shm);
  return 0;
}
