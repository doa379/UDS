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
  enqueue([this] { close(pipefd[1]);}); // Queue EOF
  th->join();
  delete th;
}

void Pqueue::reader(void)
{
  std::function<void()> *f;

  while (read(pipefd[0], &f, sizeof f) > 0)
  {
    std::function<void()> *job = static_cast<std::function<void()> *>(f);
    (*job)();
    delete job;
  }
}

bool Pqueue::enqueue(std::function<void()> job)
{
  std::function<void()> *f = new std::function<void()>(job);
  if (write(pipefd[1], &f, sizeof f) == sizeof f)
    return 1;

  return 0;
}
