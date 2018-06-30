#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <signal.h>

// У меня давно висит непроверенная синхронизация

int session_socket;
int server_socket;

void stop_server(int signal) {
  shutdown(server_socket, SHUT_RDWR);
  shutdown(session_socket, SHUT_RDWR);
  close(session_socket);
  close(server_socket);
  exit(signal);
}

void signal_handler(int signal) {
  printf("GOT SIGNAL %d, SHUTTING DOWN...\n", signal);
  stop_server(signal);
}

void listen_user() {
  char buffer[500];
  do {
    ssize_t len = recv(session_socket, buffer, 500, 0);
    if (len <= 0) break;
    buffer[len] = '\0';
    printf(buffer);
  } while (strcmp(buffer, "quit\n") != 0 && strcmp(buffer, "quit\r\n") != 0);
}

int main(int argc, char *argv[]) {
  signal(SIGINT, signal_handler);

  struct sockaddr_in server;
  socklen_t socket_size = sizeof(struct sockaddr_in);

  memset(&server, 0, sizeof(server));
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = htonl(INADDR_ANY);
  server.sin_port = htons(atoi(argv[1]));

  server_socket = socket(AF_INET, SOCK_STREAM, 0);

  if (bind(server_socket, (struct sockaddr *)&server, sizeof(struct sockaddr)) < 0) {
    perror("Bind failed\n");
    exit(EXIT_FAILURE);
  }

  if (listen(server_socket, 1)) {
    perror("Listen failed\n");
    exit(EXIT_FAILURE);
  }


  while(session_socket = accept(server_socket, NULL, &socket_size)) {
    if (fork() == 0) {
      listen_user();
      stop_server(EXIT_SUCCESS);
    } else {
      close(session_socket);
    }
  }

  close(server_socket);
  exit(EXIT_SUCCESS);
}