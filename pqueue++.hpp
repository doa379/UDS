#pragma once

#include <thread>
#include <functional>
#include <sys/types.h>

class Pqueue
{
  std::thread *th;
  int32_t pipefd[2] { 0, 0 }; /* { R, W } */
  void reader(void);

public:
  Pqueue(void);
  ~Pqueue(void);
  bool enqueue(std::function<void()>);
};
