// UNIX Domain Sockets
// $ g++ twoway.cpp -o twoway -l pthread && ./twoway

#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <string>
#include <cstdio>
#include <thread>
#include <cstdint>
#include <csignal>
#define SOCKET_PATH "/tmp"
static bool quit;

class Uds
{
protected:
  struct sockaddr_un addr;
  int32_t fd, rc, cl;
  std::string socket_path;

  public:
  Uds(void);
  ~Uds(void);
};

class Reader : public Uds
{
public:
  Reader(void) { unlink(socket_path.c_str()); };
  ~Reader(void) { close(cl); };
  void r(void);
};

class Writer : public Uds
{
public:
  bool w(void *, size_t);
  bool conn(void);
};

static Writer prod;
static Reader cons;

Uds::Uds(void)
{
  socket_path = static_cast<std::string>(SOCKET_PATH) + "/" + "twoway_socket";

  if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
  {
    perror("socket error");
    exit(-1);
  }

  memset(&addr, 0, sizeof(addr));
  addr.sun_family = AF_UNIX;
  strncpy(addr.sun_path, socket_path.c_str(), sizeof(addr.sun_path) - 1);
}

Uds::~Uds(void)
{

}

bool Writer::conn(void)
{
  if (connect(fd, (struct sockaddr *) &addr, sizeof(addr)) == -1)
    return 0;

  return 1;
}

bool Writer::w(void *data, size_t size)
{
  if ((rc = write(fd, data, size)) != size)
  {
    if (rc > 0)
      fprintf(stderr,"partial write");

    else
    {
      perror("write error");
      return 0;
    }
  }

  return 1;
}

void Reader::r(void)
{
  if (bind(fd, (struct sockaddr *) &addr, sizeof(addr)) == -1)
  {
    perror("bind error");
    return;
  }

  if (listen(fd, 5) == -1)
  {
    perror("listen error");
    return;
  }

  void *data = malloc(100);
  while (!quit)
  {
    if ((cl = accept(fd, NULL, NULL)) == -1)
    {
      perror("accept error");
      continue;
    }
  
    while ((rc = read(cl, data, sizeof(data))) > 0 && ((char *) data)[0] != '\0')
      printf("read %u bytes: %s\n", rc, (char *) data);

    if (rc == -1)
    {
      perror("read");
      break;
    }

    else if (rc == 0)
    {
      printf("EOF\n");
      break;
    }
  }

  printf("Consumer/Reader exit\n");
  free(data);
}

void reader_th(void)
{
  cons.r();
}

void signal_handler(int signum)
{
  printf("\nExit...\n");
  quit = 1;
}

int main()
{
  signal(SIGINT, signal_handler);
  std::thread reader(reader_th);

  while (!prod.conn())
    sleep(1);

  srand(0);
  void *data = malloc(100);
  while (!quit)
  {
    unsigned event = rand() % 100;
    memcpy(data, std::to_string(event).c_str(), sizeof(data));

    if (prod.w(data, sizeof(data)))
      printf("Wrote event %d\n", event);

    sleep(1);
  }
  printf("Producer/Writer exit\n");
  memcpy(data, "\0", 2);
  prod.w(data, 4);
  reader.join();
  free(data);
  return 0;
}
