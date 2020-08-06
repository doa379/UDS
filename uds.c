// UNIX Domain Sockets
// $ gcc uds.c -o uds -l pthread && ./uds

#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include <stdint.h>
#include <signal.h>
#include <stdbool.h>
#define SOCKET_PATH "/tmp"
#define PATH_LEN 128

static bool quit;

typedef struct
{
  struct sockaddr_un addr;
  int32_t fd, rc, cl;
  char socket_path[PATH_LEN];
} uds_t;

void deinit_uds(uds_t *uds)
{
  free(uds);
}

uds_t *init_uds(void)
{
  uds_t *uds = calloc(1, sizeof *uds);
  sprintf(uds->socket_path, "%s/$s", SOCKET_PATH, "twoway_socket");

  if ((uds->fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
  {
    perror("socket error");
    deinit_uds(uds);
    exit(-1);
  }

  memset(&uds->addr, 0, sizeof(uds->addr));
  uds->addr.sun_family = AF_UNIX;
  strncpy(uds->addr.sun_path, uds->socket_path, sizeof(uds->addr.sun_path) - 1);
  unlink(uds->socket_path);
  return uds;
}

bool connector(uds_t *uds)
{
  if (connect(uds->fd, (struct sockaddr *) &uds->addr, sizeof(uds->addr)) == -1)
    return 0;

  return 1;
}

bool sender(uds_t *uds, void **data)
{
  if ((uds->rc = write(uds->fd, data, sizeof(int64_t))) != sizeof(int64_t))
  {
    if (uds->rc > 0)
      fprintf(stderr,"partial write");

    else
    {
      perror("write error");
      return 0;
    }
  }

  return 1;
}

void *receiver(void *userp)
{
  uds_t *uds = userp;
  
  if (bind(uds->fd, (struct sockaddr *) &uds->addr, sizeof(uds->addr)) == -1)
  {
    perror("bind error");
    return NULL;
  }

  if (listen(uds->fd, 5) == -1)
  {
    perror("listen error");
    return NULL;
  }

  int64_t data;
  while (!quit)
  {
    if ((uds->cl = accept(uds->fd, NULL, NULL)) == -1)
    {
      perror("accept error");
      continue;
    }
  
    while ((uds->rc = read(uds->cl, &data, sizeof(data))) > 0)
    {
      printf("read %u bytes: %s\n", uds->rc, (char *) data);
      sleep(2);
      printf("read finished on %s\n", (char *) data);
      free((void *) data);
    }

    if (uds->rc == -1)
    {
      perror("read");
      break;
    }

    else if (uds->rc == 0)
    {
      printf("EOF\n");
      break;
    }
  }

  printf("Consumer/Reader exit\n");
  return NULL;
}

void signal_handler(int signum)
{
  printf("\nExit...\n");
  quit = 1;
}

int main()
{
  signal(SIGINT, signal_handler);
  uds_t *prod = init_uds(), *cons = init_uds();
  pthread_t reader_th;
  pthread_create(&reader_th, NULL, &receiver, cons);

  while (!connector(prod))
    sleep(1);

  srand(0);
  void *last_data;
  
  while (!quit)
  {
    unsigned event = rand() % 100;
    void *data = malloc(sizeof(2 * sizeof(char)));
    sprintf((char *) data, "%d", event);
    last_data = data;

    if (sender(prod, &data))
      printf("Wrote event %d\n", event);

    sleep(1);
  }
  printf("Producer/Writer exit\n");
  close(cons->cl);
  pthread_join(reader_th, NULL);
  free(last_data);
  deinit_uds(prod);
  deinit_uds(cons);
  return 0;
}
