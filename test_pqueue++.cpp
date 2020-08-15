#include <iostream>
#include <chrono>
#include "pqueue++.hpp"

void f(int arg)
{
  std::cout << "reading f(" << arg << ")\n";
  std::this_thread::sleep_for(std::chrono::seconds(2));
  std::cout << "read f(" << arg << ")\n";
}

int main()
{
  Pqueue pqueue;
  for (unsigned i = 100; i < 105; i++)
  {
    pqueue.enqueue([i]() { f(i); });
    std::cout << "wrote f(" << i << ")\n";
  }
  return 0;
}
