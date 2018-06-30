#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <zconf.h>
#include <string.h>
#include <signal.h>

#define CONN_LIMIT 4

int hostFd;
int clientFd;

void terminationHandler(int signum) {
  shutdown(clientFd, SHUT_RDWR);
  close(clientFd);
  close(hostFd);
  exit(1);
}


int main() {
  signal(SIGINT, terminationHandler);
  struct sockaddr_in server;
  char message[128] = "";

  server.sin_family = AF_INET;
  server.sin_port = htons(4242);

  hostFd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (hostFd == -1) {
    fprintf(stderr, "failed to initialize listening socket");
    return 1;
  }
  printf("socket initialized\n");

  if (bind(hostFd, (const struct sockaddr *) &server, sizeof(server)) == -1) {
    fprintf(stderr, "failed to bind listening socket");
    return 1;
  }
  printf("socket binded\n");

  if (listen(hostFd, CONN_LIMIT) == -1) {
    fprintf(stderr, "failed to bring socket into listening state");
    return 1;
  };
  printf("socket is in listening mode\n\n");

  while((clientFd = accept(hostFd, (struct sockaddr *)NULL, NULL)) != -1) {
    pid_t pid;
    if((pid = fork()) == 0) {
      ssize_t mesSize;
      while ((mesSize = recv(clientFd, message, 128, 0)) > 0) {
        message[mesSize] = '\0';
        printf("Message Received: %s\n", message);
        if (strcmp(message, "quit\n") == 0 || strcmp(message, "quit\r\n") == 0) {
          shutdown(clientFd, SHUT_RDWR);
          close(clientFd);
        }
        strcpy(message, "");
      }
      exit(0);
    }
  }

  return 0;
}