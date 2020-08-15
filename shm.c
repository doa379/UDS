#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "shm.h"

shm_t *shm_new(size_t size)
{
  shm_t *shm = malloc(sizeof *shm);
  shm->size = size;

  if ((shm->id = shmget(IPC_PRIVATE, size, IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR)) < 0)
  {
    perror("shmget");
    free(shm);
    return NULL;
  }

  return shm;
}

void shm_write(shm_t *shm, void *data)
{
  void *shm_data;

  if ((shm_data = shmat(shm->id, NULL, 0)) == (void *) -1)
  {
    perror("write");
    return;
  }

  memcpy(shm_data, data, shm->size);
  shmdt(shm_data);
}

void shm_read(void *data, shm_t *shm)
{
  void *shm_data;

  if ((shm_data = shmat(shm->id, NULL, 0)) == (void *) -1)
  {
    perror("read");
    return;
  }
  memcpy(data, shm_data, shm->size);
  shmdt(shm_data);
}

void shm_del(shm_t *shm)
{
  shmctl(shm->id, IPC_RMID, 0);
  free(shm);
}

