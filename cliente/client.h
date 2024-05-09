#pragma once
#include <winsock2.h>
// Function declarations
int initClient();
int socketCreation(SOCKET& clientSocket);
int connectionToServer(SOCKET& clientSocket);
void sendMessage(SOCKET& clientSocket, const char *message);
