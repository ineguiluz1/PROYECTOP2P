#include <winsock2.h>
#ifndef SERVER_H
#define SERVER_H

void handleConnection(SOCKET client_socket, char buffer[1024]);

#endif


