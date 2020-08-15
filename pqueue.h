#ifndef PQUEUE_H
#define PQUEUE_H

#include <stddef.h>
#include <stdbool.h>
#include <sys/types.h>
#include "shm.h"

typedef struct
{
  void (*fn)(void *);
  shm_t shm;
} job_t;

typedef struct
{
  int pipefd[2]; /* { R, W } */
  pid_t cpid;
} pqueue_t;

void pqueue_del(pqueue_t *);
pqueue_t *pqueue_new(void);
bool enqueue(pqueue_t *pqueue, void (*)(void *), shm_t *);

#endif

