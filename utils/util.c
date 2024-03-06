#include "util.h"
#include "winsock2.h"
#include <stdio.h>

void iniciar_winsock(WSADATA *wsadata){
    if(WSAStartup(MAKEWORD(2,2), wsadata) != 0)
    {
        printf("Error initializing winsock.\n");
    }
}

SOCKET create_ipv4_tcp_socket(){
    return socket(AF_INET, SOCK_STREAM, 0);
}

struct sockaddr_in create_address(int family, int PORT, const char* address){
    struct sockaddr_in addr;
    addr.sin_family = family;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = inet_addr(address);

    return addr;
}