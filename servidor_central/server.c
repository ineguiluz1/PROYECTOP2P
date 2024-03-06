#include <stdio.h>
#include "../utils/util.h"

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
    server_addr = create_address(AF_INET, 8080, "127.0.0.1");

    if(bind(listen_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) != 0)
    {
        printf("Error binding socket.\n");
    }


    return 0;
}
