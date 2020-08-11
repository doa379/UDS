#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[])
{
  int pipefd[2]; /* { R, W } */
  pid_t cpid;
  char buf[11];

  if (pipe(pipefd) == -1)
  {
    perror("pipe");
    exit(EXIT_FAILURE);
  }

  cpid = fork();
  if (cpid == -1)
  {
    perror("fork");
    exit(EXIT_FAILURE);
  }

  if (cpid == 0) 
  { /* Child reads from pipe */
    close(pipefd[1]); /* Close unused write end */

    while (read(pipefd[0], &buf, 11) > 0)
    {
      printf("%s\n", buf);
      sleep(3);
    }

    close(pipefd[0]);
    _exit(EXIT_SUCCESS);
  }
  
  else
  { /* Parent writes to pipe */
    char prod[100];
    for (unsigned i = 0; i < 4; i++)
    {
      close(pipefd[0]); /* Close unused read end */
      sprintf(prod, "Producer %d", rand() % 100);
      write(pipefd[1], prod, strlen(prod));
      sleep(1);
    }
    close(pipefd[1]); /* Reader will see EOF */
    exit(EXIT_SUCCESS);
  }
}
