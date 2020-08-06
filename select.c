#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <wait.h>
#include <signal.h>
#include <errno.h>
#include <sys/select.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
 
#define MAXBUF 256
 
void child_process(void)
{
  sleep(2);
  srandom(getpid());
  /* Create socket and connect to server */
  int sockfd = socket(AF_INET, SOCK_STREAM, 0), num = 1;
  struct sockaddr_in addr = { 0 };
  addr.sin_family = AF_INET;
  addr.sin_port = htons(2000);
  addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  connect(sockfd, (struct sockaddr *) &addr, sizeof(addr));
  printf("child {%d} connected \n", getpid());
  
  while(1)
  {
    int sl = (random() % 10 ) +  1;
    num++;
    sleep(sl);
    char msg[MAXBUF];
    sprintf (msg, "Test message %d from client %d", num, getpid());
    int n = write(sockfd, msg, strlen(msg));	/* Send message */
    (void) n;
  }
}
 
int main()
{
  for(int i = 0; i < 5; i++)
  	if(fork() == 0)
  	{
  		child_process();
  		exit(0);
  	}
 
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in addr, client;
  memset(&addr, 0, sizeof (addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(2000);
  addr.sin_addr.s_addr = INADDR_ANY;
  bind(sockfd, (struct sockaddr *) &addr, sizeof(addr));
  listen (sockfd, 5); 
  int fds[5], max = 0;

  for (int i = 0; i < 5; i++) 
  {
    memset(&client, 0, sizeof (client));
    int addrlen = sizeof(client);
    fds[i] = accept(sockfd, (struct sockaddr *) &client, &addrlen);
    if(fds[i] > max)
    	max = fds[i];
  }
  
  fd_set rset;

  while (1)
  {
	  FD_ZERO(&rset);
  	for (int i = 0; i < 5; i++)
  		FD_SET(fds[i], &rset);
 
   	puts("round again");
	  select(max + 1, &rset, NULL, NULL, NULL);
 
    for(int i = 0; i < 5; i++)
      if (FD_ISSET(fds[i], &rset))
      {
        char buffer[MAXBUF] = { };
        read(fds[i], buffer, MAXBUF);
        puts(buffer);
      }
  }
  return 0;
}
