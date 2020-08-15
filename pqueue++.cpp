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
