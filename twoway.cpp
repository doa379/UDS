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
#define SOCKET_PATH "/tmp/"
static bool quit;

class Uds
{
  struct sockaddr_un addr;
  int32_t fd, rc, cl;
  std::string socket_path, buf;

  public:
  Uds(std::string);
  ~Uds(void);
  bool writer(std::string);
  bool writer_connector(void);
  void reader(void);
};

Uds::Uds(std::string name)
{
  socket_path = SOCKET_PATH + name;

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

bool Uds::writer_connector(void)
{
  if (connect(fd, (struct sockaddr *) &addr, sizeof(addr)) == -1)
    return 0;

  return 1;
}

bool Uds::writer(std::string buf)
{
  if ((rc = write(fd, buf.c_str(), buf.size() * sizeof(char))) != buf.size())
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

void Uds::reader(void)
{
  unlink(socket_path.c_str());

  if (bind(fd, (struct sockaddr *) &addr, sizeof(addr)) == -1)
  {
    perror("bind error");
    exit(-1);
  }

  if (listen(fd, 5) == -1)
  {
    perror("listen error");
    exit(-1);
  }

  while (!quit)
  {
    if ((cl = accept(fd, NULL, NULL)) == -1)
    {
      perror("accept error");
      continue;
    }

    while ((rc = read(cl, &buf[0], sizeof(buf))) > 0 && !quit)
    {
      //buf[rc] = 0x00;
      printf("read %u bytes: %*s\n", rc, rc, buf.c_str());
    }

    if (rc == -1)
    {
      perror("read");
      exit(-1);
    }

    else if (rc == 0)
    {
      printf("EOF\n");
      close(cl);
    }
  }

  close(cl);
  printf("Reader exit\n");
}

void writer_th(void)
{
  Uds w("name");
  // Send events
  srand(0);

  while (!w.writer_connector())
    sleep(1);

  while (!quit)
  {
    unsigned event = rand() % 100;

    if (w.writer(std::to_string(event)))
      printf("Wrote %u\n", event);

    sleep(5);
  }

  w.writer("0x00");
  printf("Writer exit\n");
}

void reader_th(void)
{
  Uds r("name");
  r.reader();
}

void signal_handler(int signum)
{
  printf("\nExit...\n");
  quit = 1;
}

int main()
{
  signal(SIGINT, signal_handler);
  std::thread writer(writer_th);
  std::thread reader(reader_th);
  writer.join();
  reader.join();
  return 0;
}
