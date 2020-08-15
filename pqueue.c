/* Pipe Queue */
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "pqueue.h"

void pqueue_del(pqueue_t *pqueue)
{
  close(pqueue->pipefd[1]); /* Reader will see EOF */
  int status;
  while (wait(&status) != pqueue->cpid);
  free(pqueue);
}

pqueue_t *pqueue_new(void)
{
  pqueue_t *pqueue = calloc(1, sizeof *pqueue);
  
  if (pipe(pqueue->pipefd) == -1)
  { /* pipe error */
    free(pqueue);
    return NULL;;
  }

  else if ((pqueue->cpid = fork()) == -1)
  { /* fork error */
    free(pqueue);
    return NULL;
  }

  else if (pqueue->cpid == 0)
  { /* Child reads from pipe */
    close(pqueue->pipefd[1]); /* Close unused write end */
    job_t job;

    while (read(pqueue->pipefd[0], &job, sizeof job) > 0)
    {
      void *data = malloc(job.shm.size);
      shm_read(data, &job.shm);
      (job.fn)(data);
      shm_write(&job.shm, data);
      free(data);
    }
  
    _exit(0);
  }
  
  return pqueue;
}

bool enqueue(pqueue_t *pqueue, void (*fn)(void *), shm_t *shm)
{
  job_t job = { fn, *shm };
  close(pqueue->pipefd[0]); /* Close unused read end */
  if (write(pqueue->pipefd[1], &job, sizeof job) == sizeof job)
    return 1;

  return 0;
}
