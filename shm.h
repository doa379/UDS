#ifndef SHM_H
#define SHM_H

#include <stddef.h>

typedef struct
{
  int id;
  size_t size;
} shm_t;


shm_t *shm_new(void *, size_t);
void shm_write(shm_t *, void *);
void shm_read(void *, shm_t *);
void shm_del(shm_t *);

#endif
