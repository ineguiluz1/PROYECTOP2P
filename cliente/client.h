#pragma once
#include <winsock2.h>
// Function declarations
int initClient();
int socketCreation(SOCKET& clientSocket);
int connectionToServer(SOCKET &clientSocket, const char *ip, int port);
void sendMessage(SOCKET& clientSocket, const char *message);
void sendFile(SOCKET& clientSocket, const char *filePath);
void conectarConCliente(SOCKET &clientSocketTransferencia, const char *ip,int port);
