#pragma once
#include <winsock2.h>
// Function declarations
int initClient();
int socketCreation(SOCKET& clientSocket);
int connectionToServer(SOCKET &clientSocket, const char *ip, int port);
void sendMessage(SOCKET& clientSocket, const char *message);
void conectarConCliente(SOCKET &clientSocketTransferencia, const char *ip,int port);
void receiveFile(SOCKET& clientSocket, const char *fileName,char *buffer, size_t bufferSize);
