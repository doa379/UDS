#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "squeue.h"

int main()
{
  squeue_t *squeue = squeue_new();

  for (unsigned i = 0; i < 4; i++)
  {
    unsigned event = rand() % 100;

    if (enqueue(squeue, &event, sizeof(int)))
      printf("Sent event %d\n", event);

    sleep(1);
  }

  squeue_del(squeue);
  return 0;
}
