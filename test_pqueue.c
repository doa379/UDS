#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "pqueue.h"
#include "shm.h"

void f(void *arg)
{
  printf("Consumer: %d\n", *(int *) arg);
  int *n = (int *) arg;
  (*n)++;
}

int main(int argc, char *argv[])
{
  int A[] = { 100, 200, 300, 400 };
  pqueue_t *pqueue = pqueue_new();
  shm_t *shm[4];
  for (unsigned i = 0; i < 4; i++)
  {
    shm[i] = shm_new(sizeof A[i]);
    shm_write(shm[i], &A[i]);
    while (!enqueue(pqueue, f, shm[i]));
  }

  pqueue_del(pqueue);
  for (unsigned i = 0; i < 4; i++)
  {
    shm_read(&A[i], shm[i]);
    printf("Modified A[%d] = %d\n", i, A[i]);
    shm_del(shm[i]);
  }

  return 0;
}
