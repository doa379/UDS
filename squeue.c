//#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
//#include <fcntl.h>
//#include <sys/ioctl.h>
#include "squeue.h"

static bool connector(uds_t *uds)
{
  if (connect(uds->fd, (struct sockaddr *) &uds->addr, sizeof(uds->addr)) == -1)
    return 0;

  return 1;
}

static bool sender(uds_t *uds, void **data)
{
  if ((uds->rc = write(uds->fd, data, sizeof(int64_t))) != sizeof(int64_t))
  {
    if (uds->rc > 0)
    // partial write
      ;

    else
    // write error
      return 0;
  }

  return 1;
}

// Writer
bool enqueue(squeue_t *squeue, void *arg, size_t size)
{
  void *data = calloc(1, size);
  memcpy(data, arg, size);

  if (sender(squeue->prod, &data))
    return 1;
  
  return 0;
}

// Reader
static void *worker_th(void *userp)
{
  squeue_t *squeue = userp;
  uds_t *cons = squeue->cons;
  
  if (bind(cons->fd, (struct sockaddr *) &cons->addr, sizeof(cons->addr)) == -1)
  // "bind error"
    return NULL;

  else if (listen(cons->fd, 5) == -1)
  // "listen error");
    return NULL;
  
  else if (!connector(squeue->prod))
    return NULL;

  int64_t data;

  while (1)
  {
    if ((cons->cl = accept(cons->fd, NULL, NULL)) == -1)
    // "accept error"
      continue;
  
    while ((cons->rc = read(cons->cl, &data, sizeof(data))) > 0)
    {
      printf("read %u bytes: %d\n", cons->rc, *(int *) data);
      sleep(3);
      printf("read finished on %d\n", *(int *) data);

      if (*(int *) data == 0 && squeue->quit)
      {
        free((void *) data);
        return NULL;
      }

      free((void *) data);
    }

    if (cons->rc == -1)
    // read error
      break;

    else if (cons->rc == 0)
    // EOF
      break;
  }

  return NULL;
}
/*
int count(uds_t *uds)
{
  int size = 0;
  
  //while (!size && ioctl(uds->fd, FIONREAD, &size) > -1)
    //sleep(1);
  ioctl(uds->fd, FIONREAD, &size);
  return size;
}
*/
static void deinit_uds(uds_t *uds)
{
  // Remove file uds->socket_path if it exists
  close(uds->cl);
  free(uds);
}

static uds_t *init_uds(void)
{
  uds_t *uds = calloc(1, sizeof *uds);
  sprintf(uds->socket_path, "%s/%s", SOCKET_PATH, "twoway_socket");
  unlink(uds->socket_path);

  if ((uds->fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
  {
  // socket error
    deinit_uds(uds);
    return NULL;
  }

  memset(&uds->addr, 0, sizeof(uds->addr));
  uds->addr.sun_family = AF_UNIX;
  strncpy(uds->addr.sun_path, uds->socket_path, sizeof(uds->addr.sun_path) - 1);
  return uds;
}

void squeue_del(squeue_t *squeue)
{
  squeue->quit = 1;
  // Dummy signal
  enqueue(squeue, NULL, 0);
  pthread_join(squeue->pth, NULL);
  deinit_uds(squeue->prod);
  deinit_uds(squeue->cons);
  free(squeue);
  squeue = NULL;
}

squeue_t *squeue_new(void)
{
  squeue_t *squeue = calloc(1, sizeof *squeue);

  if (squeue == NULL)
    return NULL;

  else if (!(squeue->cons = init_uds()))
  {
    free(squeue);
    return NULL;
  }

  else if (!(squeue->prod = init_uds()))
  {
    deinit_uds(squeue->cons);
    free(squeue);
    return NULL;
  }

  pthread_create(&squeue->pth, NULL, worker_th, (void *) squeue);
  while (!enqueue(squeue, NULL, 0));
  return squeue;
}
