#include "util.h"
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

struct AcceptedSocket {
  int *accepted_socket_fd;
  struct sockaddr_in address;
  int error;
  bool accept_successful;
};

struct AcceptedSocket accepted_sockets[10];
int accepted_sockets_count = 0;


/*
 * accept_connection
 *
 * Description: Accepts a connection from a client on the specified server
 * socket.
 *
 * Parameters:
 *  - server_socket_fd: Integer representing the file descriptor of the server
 * socket.
 *
 * Returns:
 *  - Pointer to a struct AcceptedSocket representing the accepted connection.
 *    The struct contains information about the accepted socket, including its
 * file descriptor, address details, and whether the acceptance was successful
 * or not. Returns NULL if the connection acceptance fails.
 *
 * Note: Memory management responsibility for the returned struct and its
 * contents lies with the caller of this function.
 */
struct AcceptedSocket *accept_connection(int server_socket_fd) {
  struct sockaddr_in client_address;
  socklen_t client_address_size = sizeof(struct sockaddr_in);

  // Accept connection from client
  int *client_socket_fd =
      (int *)malloc(sizeof(int)); // Need to be pointer to pass into thread
  *client_socket_fd =
      accept(server_socket_fd, (struct sockaddr *)&client_address,
             &client_address_size);
  struct AcceptedSocket *accepted_socket =
      malloc(sizeof(struct AcceptedSocket));

  accepted_socket->address = client_address;
  accepted_socket->accepted_socket_fd = client_socket_fd;
  accepted_socket->accept_successful = *client_socket_fd > 0;

  if (!accepted_socket->accept_successful)
    accepted_socket->error = *client_socket_fd;

  return accepted_socket;
}

void broadcast_client_message(char *buffer, int socket_fd) {
  for (int i = 0; i < accepted_sockets_count; i++) {
    if (*accepted_sockets[i].accepted_socket_fd != socket_fd) {
      send(*accepted_sockets[i].accepted_socket_fd, buffer, strlen(buffer), 0);
    }
  }
}

void *handle_client(void *args) {
  int client_socket_fd = *((int *)args);
  char buffer[1024];
  while (true) {
    ssize_t amount_received = recv(client_socket_fd, buffer, sizeof buffer, 0);
    if (amount_received > 0) {
      buffer[amount_received] = '\0'; // Prevents printf reading beyond buffer
      printf("Received data: %s\n", buffer);

      // Broadcast to all connected clients
      broadcast_client_message(buffer, client_socket_fd);
    }
    // When user kills client connection, the amount recieved is 0;
    if (amount_received == 0)
      break;
  }
  close(client_socket_fd);
  return NULL;
}

int main(void) {
  int server_socket_fd = create_TCP_IPv4_socket();
  struct sockaddr_in *server_address = create_IPv4_address("", 2000);

  // Bind socket to server's address
  int bind_result = bind(server_socket_fd, (struct sockaddr *)server_address,
                         sizeof(*server_address));
  if (bind_result == 0)
    printf("Socket bind was successful\n");
  else {
    perror("Bind error");
    exit(EXIT_FAILURE);
  }

  // Start listening for incoming connections
  int listen_result = listen(server_socket_fd, 10);
  if (listen_result < 0) {
    perror("Listen error");
    exit(EXIT_FAILURE);
  }

  // Accept multiple clients
  while (true) {
    // Accept incoming connection from client
    struct AcceptedSocket *client_socket = accept_connection(server_socket_fd);
    if (client_socket->error < 0) {
      perror("Accept error");
      continue;
    }
    accepted_sockets[accepted_sockets_count++] = *client_socket;

    // Thread client interaction
    pthread_t th;
    if (pthread_create(&th, NULL, handle_client,
                       client_socket->accepted_socket_fd) != 0) {
      perror("Thread creation error");
    }
  }

  // Close client socket and shutdown server socket
  shutdown(server_socket_fd, SHUT_RDWR);

  return 0;
}
