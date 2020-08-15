/* Pipe Queue using pthreads */

#include <unistd.h>
#include "pqueue++.hpp"

Pqueue::Pqueue(void)
{
  pipe(pipefd);
  th = new std::thread(&Pqueue::reader, this);
}

Pqueue::~Pqueue(void)
{
  close(pipefd[1]);
  th->join();
  delete th;
}

void Pqueue::reader(void)
{
  //close(pipefd[1]);
  std::function<void()> job;

  while (read(pipefd[0], &job, sizeof job) > 0)
  {
    job();
  }
}

bool Pqueue::enqueue(std::function<void()> job)
{
  //close(pipefd[0]);
  if (write(pipefd[1], &job, sizeof job) == sizeof job)
    return 1;

  return 0;
}
/*
void pqueue_del(pqueue_t *pqueue)
{
  close(pqueue->pipefd[1]);
  int status;
  while (wait(&status) != pqueue->cpid);
  free(pqueue);
}

pqueue_t *pqueue_new(void)
{
  pqueue_t *pqueue = calloc(1, sizeof *pqueue);
  
  if (pipe(pqueue->pipefd) == -1)
  {
    free(pqueue);
    return NULL;;
  }

  else if ((pqueue->cpid = fork()) == -1)
  {
    free(pqueue);
    return NULL;
  }

  else if (pqueue->cpid == 0)
  {
    close(pqueue->pipefd[1]);
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
  close(pqueue->pipefd[0]);
  if (write(pqueue->pipefd[1], &job, sizeof job) == sizeof job)
    return 1;

  return 0;
}
*/
