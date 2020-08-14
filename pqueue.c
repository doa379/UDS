/* Pipe Queue */
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "pqueue.h"
#include <stdio.h>

void pqueue_del(pqueue_t *pqueue)
{
  close(pqueue->pipefd[1]); /* Reader will see EOF */
}

pqueue_t *pqueue_new(void)
{
  pqueue_t *pqueue = calloc(1, sizeof *pqueue);
  
  if (pipe(pqueue->pipefd) == -1)
  { /* pipe error */
    free(pqueue);
    return NULL;;
  }

  pid_t cpid = fork();
  if (cpid == -1)
  { /* fork error */
    free(pqueue);
    return NULL;
  }

  else if (cpid == 0)
  { /* Child reads from pipe */
    close(pqueue->pipefd[1]); /* Close unused write end */
    job_t job;

    while (read(pqueue->pipefd[0], &job, sizeof job) > 0)
    {
      //printf("%p %p %ld\n", job.fn, job.arg, job.arg_size);
      printf("%s\n", (char *) job.arg);
      //(job.fn)(job.arg);

      //if (job.arg_size)
        //free(job.arg);
    }

    close(pqueue->pipefd[0]);
    free(pqueue);
    return NULL;
  }

  return pqueue;
}

bool enqueue(pqueue_t *pqueue, void (*fn)(void *), void *arg, size_t size)
{
  void *data = arg;

  if (size)
  {
    data = calloc(1, size);
    memcpy(data, arg, size);
  }

  job_t job = { .fn = fn, .arg = data, .arg_size = size };
  //close(pqueue->pipefd[0]); /* Close unused read end */
  if (write(pqueue->pipefd[1], &job, sizeof job) == sizeof job)
    return 1;

  else if (size)
    free(data);

  return 0;
}

