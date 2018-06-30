#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <signal.h>

#define PORTNUM 2300

int consocket;
int mysocket;

void signal_handler(int signal) {
  printf("GOT SIGNAL %d, SHUTTING DOWN...\n", signal);
  shutdown(consocket, SHUT_RDWR);
  close(consocket);
  close(mysocket);
  exit(signal);
}

int main(int argc, char *argv[])
{
  signal(SIGINT, signal_handler);

  struct sockaddr_in dest; /* socket info about the machine connecting to us */
  struct sockaddr_in serv; /* socket info about our server */
  socklen_t socksize = sizeof(struct sockaddr_in);

  memset(&serv, 0, sizeof(serv));           /* zero the struct before filling the fields */
  serv.sin_family = AF_INET;                /* set the type of connection to TCP/IP */
  serv.sin_addr.s_addr = htonl(INADDR_ANY); /* set our address to any interface */
  serv.sin_port = htons(PORTNUM);           /* set the server port number */

  mysocket = socket(AF_INET, SOCK_STREAM, 0);

  /* bind serv information to mysocket */
  if(bind(mysocket, (struct sockaddr *)&serv, sizeof(struct sockaddr)) < 0) {
    perror("Bind failed\n");
    exit(EXIT_FAILURE);
  }

  /* start listening, allowing a queue of up to 1 pending connection */
  listen(mysocket, 1);


  char buffer[500];

  while(consocket = accept(mysocket, (struct sockaddr *)&dest, &socksize)) {
    printf("while consocket\n");
    if (fork() == 0) {
      do {
        ssize_t len = recv(consocket, buffer, 500, 0);
        if (len <= 0) break;
        buffer[len] = '\0';
        printf(buffer);
      } while (strcmp(buffer, "quit\n") != 0 && strcmp(buffer, "quit\r\n") != 0);
      shutdown(consocket, SHUT_RDWR);
      close(consocket);
      close(mysocket);
      return EXIT_SUCCESS;
    }
    close(consocket);
  }

  close(mysocket);
  return EXIT_SUCCESS;
}