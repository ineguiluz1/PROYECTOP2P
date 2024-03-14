#include <stdio.h>
#include <stdbool.h>
#include "../utils/util.h"
#include <ws2tcpip.h>
#include <string.h>
#define BACKLOG 12
#define PORT 8080

int main(int argc, char const *argv[])
{
    // Inicializar Winsock.

    WSADATA wsadata;
    iniciar_winsock(&wsadata);

    // Crear el socket del servidor.

    SOCKET client_socket;
    client_socket = create_ipv4_tcp_socket();

    // Crear la dirección del servidor.

    struct sockaddr_in server_addr;
    server_addr = create_address(AF_INET, PORT, "127.0.0.1");

    if(connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) != 0)
    {
        printf("Error connecting to server.\n");
    }
        
    
    printf("Client connected.\n");

    char buffer[1024];

    printf("Enviar mensaje a servidor: ");
    char mssg[1024] = {0};

    fgets(buffer, 1024, stdin);
    strcpy(mssg, buffer);

    if(send(client_socket, mssg, strlen(mssg), 0) != 0)
    {
        printf("Mensaje enviado: ");
    }

    return 0;
}
