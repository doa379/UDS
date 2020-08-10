#ifndef SQUEUE_H
#define SQUEUE_H

#include <sys/socket.h>
#include <sys/un.h>
#include <pthread.h>
#include <stdint.h>
#include <stdbool.h>

#define SOCKET_PATH "/tmp"
#define PATH_LEN 128

typedef struct
{
  struct sockaddr_un addr;
  int32_t fd, rc, cl;
  char socket_path[PATH_LEN];
} uds_t;

typedef struct
{
  bool quit;
  uds_t *prod, *cons;
  pthread_t pth;
} squeue_t;


bool enqueue(squeue_t *, void *, size_t);
int count(uds_t *);
void squeue_del(squeue_t *);
squeue_t *squeue_new(void);

#endif
