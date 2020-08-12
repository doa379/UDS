#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "pqueue.h"

void f(void *arg)
{
  printf("Consumer: %s\n", (char *) arg);
}

int main(int argc, char *argv[])
{
  pqueue_t *pqueue = pqueue_new(); 
  for (unsigned i = 0; i < 4; i++)
  {
    char prod[100];
    sprintf(prod, "Producer %d", rand() % 100);
    enqueue(pqueue, f, prod, strlen(prod));
    sleep(1);
  }

  pqueue_del(pqueue);
  return 0;
}
