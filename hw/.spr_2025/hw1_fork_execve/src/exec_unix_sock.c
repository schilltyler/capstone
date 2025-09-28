#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <sys/wait.h>
#include <unistd.h>

#define SOCK_PATH "/tmp/output.sock"
#define BUFFER_SIZE 1024

/*
================================================================================
Part 1: Process Creation and I/O Redirection (unix socket Version for AArch64)
================================================================================
*/
extern char **environ;

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <executable> [args...]\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  // <soludion>
  // Create server socket
  int server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
  if (server_fd == -1) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  struct sockaddr_un addr = {
      .sun_family = AF_UNIX,
  };
  strncpy(addr.sun_path, SOCK_PATH, sizeof(addr.sun_path) - 1);

  unlink(SOCK_PATH); // Remove if exists
  if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
    perror("bind");
    close(server_fd);
    exit(EXIT_FAILURE);
  }

  if (listen(server_fd, 1) == -1) {
    perror("listen");
    close(server_fd);
    unlink(SOCK_PATH);
    exit(EXIT_FAILURE);
  }

  pid_t pid = fork();
  if (pid == -1) {
    perror("fork");
    close(server_fd);
    unlink(SOCK_PATH);
    exit(EXIT_FAILURE);
  }

  if (pid == 0) {     // Child
    close(server_fd); // Don't need server socket

    // Connect to the server
    int client_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (client_fd == -1) {
      perror("socket");
      exit(EXIT_FAILURE);
    }

    if (connect(client_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
      perror("connect");
      close(client_fd);
      exit(EXIT_FAILURE);
    }

    // Redirect stdout/stderr to the socket
    if (dup2(client_fd, STDOUT_FILENO) == -1 ||
        dup2(client_fd, STDERR_FILENO) == -1) {
      perror("dup2");
      exit(EXIT_FAILURE);
    }
    close(client_fd); // Original fd no longer needed

    execvp(argv[1], &argv[1]);
    perror("execvp");
    exit(EXIT_FAILURE);
  }

  // Parent - accept and read from socket
  int conn_fd = accept(server_fd, NULL, NULL);
  if (conn_fd == -1) {
    perror("accept");
    close(server_fd);
    unlink(SOCK_PATH);
    exit(EXIT_FAILURE);
  }
  close(server_fd); // No more connections needed

  char buffer[BUFFER_SIZE];
  ssize_t nread;
  while ((nread = read(conn_fd, buffer, BUFFER_SIZE)) > 0) {
    write(STDOUT_FILENO, buffer, nread);
  }

  close(conn_fd);
  unlink(SOCK_PATH);
  wait(NULL);

  // </solution>
  return 0;
}
