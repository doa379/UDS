#ifndef PQUEUE_H
#define PQUEUE_H

#include <stddef.h>
#include <stdbool.h>

typedef struct
{
  void (*fn)(void *);
  void *arg;
  size_t arg_size;
} job_t;

typedef struct
{
  int pipefd[2]; /* { R, W } */
} pqueue_t;

void pqueue_del(pqueue_t *);
pqueue_t *pqueue_new(void);
bool enqueue(pqueue_t *, void (*)(void *), void *, size_t);

#endif

