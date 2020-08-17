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
  void *job = NULL;

  while (read(pipefd[0], &job, sizeof(int64_t)) > 0)
  {
    std::function<void()> *f = static_cast<std::function<void()> *>(job);
    (*f)();
    delete f;
  }
}

bool Pqueue::enqueue(void *job)
{
  if (write(pipefd[1], job, sizeof(int64_t)) == sizeof(int64_t))
    return 1;

  return 0;
}
