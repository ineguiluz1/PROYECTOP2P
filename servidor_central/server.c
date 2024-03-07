#include <stdio.h>
#include <stdbool.h>
#include "../utils/util.h"
#include <ws2tcpip.h>
#define BACKLOG 12
#define PORT 8080

int main(int argc, char const *argv[])
{
    // Inicializar Winsock.

    WSADATA wsadata;
    iniciar_winsock(&wsadata);

    // Crear el socket del servidor.

    SOCKET listen_socket;
    listen_socket = create_ipv4_tcp_socket();

    // Crear la dirección del servidor.

    struct sockaddr_in server_addr;
    server_addr = create_address(AF_INET, PORT, "127.0.0.1");

    // Bindear el socket a su dirección correspondiente.

    if(bind(listen_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) != 0)
    {
        printf("Error binding socket.\n");
    }

    // Poner el socket a escuchar. (Conexiones máximas --> BACKLOG)

    if(listen(listen_socket, BACKLOG) != 0)
    {
        printf("Error listening.\n");
    }

    printf("Server listening in port %d.\n", PORT);

    // Address del socket del cliente.

    struct sockaddr_in clientaddr;
    socklen_t len = sizeof(clientaddr);

    while(true){

        SOCKET client_socket = accept(listen_socket, (struct sockaddr*)&clientaddr, &len);

    }


    return 0;
}
