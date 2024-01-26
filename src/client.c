#include "util.h"
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void* listen_and_print_messages(void* args) {
  int socket_fd = *((int *)args);
  char buffer[1024];
  while (true) {
    ssize_t amount_recieved = recv(socket_fd, buffer, sizeof buffer, 0);
    if (amount_recieved > 0) {
      buffer[amount_recieved] = 0;
      printf("Response was: %s\n", buffer);
    }
    if (amount_recieved == 0)
      break;
  }
  close(socket_fd);
  return NULL;
}

int main() {
  int socket_fd = create_TCP_IPv4_socket();
  struct sockaddr_in *address = create_IPv4_address("127.0.0.1", 2000);

  int result = connect(socket_fd, (struct sockaddr *)address, sizeof(*address));

  if (result == 0)
    printf("Connection was successful\n");

  char *message = NULL;
  size_t message_size = 0;
  printf("Type and send a message\n");

  pthread_t th;
  pthread_create(&th, NULL, listen_and_print_messages, (void*)&socket_fd);

  while (true) {
    ssize_t char_count = getline(&message, &message_size, stdin);
    if (char_count > 0) {
      if (strcmp(message, "exit\n") == 0)
        break;
      send(socket_fd, message, char_count, 0);
    }
  }

  close(socket_fd);
  free(address);

  return 0;
}
