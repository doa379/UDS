#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include "squeue.h"

static bool connector(uds_t *uds)
{
  if (connect(uds->fd, (struct sockaddr *) &uds->addr, sizeof(uds->addr)) == -1)
    return 0;

  return 1;
}

static bool sender(uds_t *uds, job_t *job)
{
  if ((uds->rc = write(uds->fd, job, sizeof *job)) != sizeof *job)
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
bool enqueue(squeue_t *squeue, void (*fn)(void *), void *arg, size_t size)
{
  void *data = arg;

  if (size)
  {
    data = calloc(1, size);
    memcpy(data, arg, size);
  }

  job_t job = { .fn = fn, .arg = data, .size = size };

  if (sender(squeue->prod, &job))
    return 1;
 
  if (size)
    free(data);

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

  job_t job;

  while (1)
  {
    if ((cons->cl = accept(cons->fd, NULL, NULL)) == -1)
    // "accept error"
      continue;
  
    while ((cons->rc = read(cons->cl, &job, sizeof job)) > 0)
    {
      if (job.fn == NULL && squeue->quit)
      {
        if (job.size)
          free(job.arg);

        return NULL;
      }

      else if (job.fn != NULL)
        (job.fn)(job.arg);

      if (job.size)
        free(job.arg);
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

static void deinit_uds(uds_t *uds)
{
  close(uds->cl);
  free(uds);
}

static uds_t *init_uds(const char *sock_name)
{
  uds_t *uds = calloc(1, sizeof *uds);

  if ((uds->fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
  {
  // socket error
    free(uds);
    return NULL;
  }

  memset(&uds->addr, 0, sizeof(uds->addr));
  uds->addr.sun_family = AF_UNIX;
  strncpy(uds->addr.sun_path, sock_name, sizeof(uds->addr.sun_path) - 1);
  return uds;
}

void squeue_del(squeue_t *squeue)
{
  squeue->quit = 1;
  // Dummy signal
  enqueue(squeue, NULL, NULL, 0);
  pthread_join(squeue->pth, NULL);
  deinit_uds(squeue->prod);
  deinit_uds(squeue->cons);
  unlink(squeue->socket_name);
  free(squeue);
  squeue = NULL;
}

squeue_t *squeue_new(void)
{
  squeue_t *squeue = calloc(1, sizeof *squeue);

  if (squeue == NULL)
    return NULL;
  
  else if (mkdir(SOCKET_PATH, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1 
    && errno != EEXIST)
  {
    free(squeue);
    return NULL;
  }

  sprintf(squeue->socket_name, "%s/sock-%p", SOCKET_PATH, squeue);

  if (!(squeue->cons = init_uds(squeue->socket_name)))
  {
    free(squeue);
    return NULL;
  }

  else if (!(squeue->prod = init_uds(squeue->socket_name)))
  {
    deinit_uds(squeue->cons);
    free(squeue);
    return NULL;
  }

  pthread_create(&squeue->pth, NULL, worker_th, (void *) squeue);
  while (!enqueue(squeue, NULL, NULL, 0));
  return squeue;
}
