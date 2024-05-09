#include <winsock2.h>
#ifndef SERVER_H
#define SERVER_H

// Function declarations
int initServer();
int socketCreation(SOCKET& serverSocket);
int bindSocket(SOCKET& serverSocket);
int connectionsManagement(SOCKET& serverSocket,SOCKADDR_IN& client_addr);
int sendAndReceive(SOCKET& clientSocket);
void handleClient(SOCKET& clientSocket);

#endif


