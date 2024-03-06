#ifndef UTIL_H
#define UTIL_H

#include <winsock2.h>

// Función para iniciar winsock.
void iniciar_winsock(WSADATA *wsadata);

// Función para crear un socket TCP con IPv4.
SOCKET create_ipv4_tcp_socket();

// Función para crear una dirección.
struct sockaddr_in create_address(int family, int PORT, const char* address);

#endif