#include <iostream>
#include "pqueue++.hpp"

void f(int arg)
{
  std::cout << "f(" << arg << ") called\n";
}

int main()
{
  Pqueue pqueue;
  for (unsigned i = 100; i < 105; i++)
    pqueue.enqueue([i]() { f(i); });
  return 0;
}
