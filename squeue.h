#ifndef SQUEUE_H
#define SQUEUE_H

#include <sys/socket.h>
#include <sys/un.h>
#include <pthread.h>
#include <stdint.h>
#include <stdbool.h>

#define SOCKET_PATH "/tmp/squeue_sockets"
#define PATH_LEN 128

typedef struct
{
  void (*fn)(void *);
  void *arg;
  size_t size;
} job_t;

typedef struct
{
  struct sockaddr_un addr;
  int32_t fd, rc, cl;
} uds_t;

typedef struct
{
  bool quit;
  char socket_name[PATH_LEN];
  uds_t *prod, *cons;
  pthread_t pth;
} squeue_t;


bool enqueue(squeue_t *, void (*)(void *), void *, size_t);
void squeue_del(squeue_t *);
squeue_t *squeue_new(void);

#endif
