#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "squeue.h"

void f(void *arg)
{
  printf("Received event %d\n", *(int *) arg);
}

int main()
{
  squeue_t *squeue = squeue_new();

  for (unsigned i = 0; i < 4; i++)
  {
    unsigned event = rand() % 100;

    if (enqueue(squeue, f, &event, sizeof(int)))
      printf("Sent event %d\n", event);

    else
      printf("enqueue error\n");

    //printf("Jobs %d %d\n", count(squeue->prod), count(squeue->cons));
    sleep(1);
  }

  sleep(2);
  //printf("Jobs %d %d\n", count(squeue->prod), count(squeue->cons));
  squeue_del(squeue);
  return 0;
}
