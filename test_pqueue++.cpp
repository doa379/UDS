#include <iostream>
#include <chrono>
#include "pqueue++.hpp"

void g(int arg, int *p)
{
  std::cout << "reading f(" << arg << ", " << *p << ")\n";
  std::this_thread::sleep_for(std::chrono::seconds(1));
  std::cout << "read f(" << arg << ", " << *p << ")\n";
}

int main()
{
  Pqueue pqueue;
  int *p = new int(5);
  for (unsigned i = 100; i < 105; i++)
  {
    std::function<void()> *f = new std::function<void()>([=] { g(i, p); });
    if (pqueue.enqueue(&f))
      std::cout << "wrote f(" << i << ", " << *p << ")\n";
  }

  std::this_thread::sleep_for(std::chrono::seconds(10));
  // Finish
  delete p;
  return 0;
}
