#ifndef UTIL_H
#define UTIL_H

#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>

int create_TCP_IPv4_socket();
struct sockaddr_in *create_IPv4_address(char *ip, int port);

#endif
