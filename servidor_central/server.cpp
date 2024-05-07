#include <iostream>
#include <cstring>
#include "../utils/util.h"
#include "../BD/bd.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <libpq-fe.h>

#define BACKLOG 12
#define PORT 8080

void handleConnection(SOCKET client_socket, char buffer[1024]);

int main(int argc, char const *argv[])
{

    // Initialize the database
    PGconn *conn;
    conexionBD(&conn);

    // Initialize Winsock
    WSADATA wsadata;
    iniciar_winsock(&wsadata);

    // Create the buffer for communication between client and server
    char buffer[1024] = {0};

    // Create the server socket
    SOCKET listen_socket;
    listen_socket = create_ipv4_tcp_socket();

    // Create the server address
    struct sockaddr_in server_addr;
    server_addr = create_address(AF_INET, PORT, "127.0.0.1");

    // Bind the socket to its corresponding address
    if (bind(listen_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) != 0)
    {
        std::cerr << "Error binding socket." << std::endl;
        return 1;
    }

    // Put the socket in listening mode (maximum connections --> BACKLOG)
    if (listen(listen_socket, BACKLOG) != 0)
    {
        std::cerr << "Error listening." << std::endl;
        return 1;
    }

    std::cout << "Server listening on port " << PORT << "." << std::endl;

    // Address of the client socket
    struct sockaddr_in clientaddr;
    socklen_t len = sizeof(clientaddr);

    while (true)
    {
        SOCKET client_socket;
        client_socket = accept(listen_socket, (struct sockaddr*)&clientaddr, &len);
        std::cout << "Client connected." << std::endl;

        handleConnection(client_socket, buffer);
    }

    return 0;
}

void handleConnection(SOCKET client_socket, char buffer[1024], PGconn *conn)
{
    int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);

    if (bytes_received == SOCKET_ERROR)
    {
        std::cerr << "Error receiving message." << std::endl;
        closesocket(client_socket);
        return;
    }

    buffer[bytes_received] = '\0';

    //TODO: Implement handling of received commands
    if (strcmp(buffer, "LOGIN") == 0)
    {
        std::cout << "Received LOGIN command." << std::endl;
        // Implement login logic here
        // Example: send(client_socket, "Login successful", strlen("Login successful"), 0);
    }
    else if (strcmp(buffer, "REGISTER") == 0)
    {
        std::cout << "Received REGISTER command." << std::endl;
        // Implement register logic here
        // Example: send(client_socket, "Registration successful", strlen("Registration successful"), 0);
    }
    else if (strcmp(buffer, "LOGOUT") == 0)
    {
        std::cout << "Received LOGOUT command." << std::endl;
        // Implement logout logic here
        // Example: send(client_socket, "Logout successful", strlen("Logout successful"), 0);
    }
    else if (strcmp(buffer, "NODO") == 0)
    {
        std::cout << "Received NODO command." << std::endl;
        // Implement NODO command logic here
    }
    else if (strcmp(buffer, "AÑADIR_NODO") == 0)
    {
        std::cout << "Received AÑADIR_NODO command." << std::endl;
        // Implement AÑADIR_NODO command logic here
    }
    else if (strcmp(buffer, "ELIMINAR_NODO") == 0)
    {
        std::cout << "Received ELIMINAR_NODO command." << std::endl;
        // Implement ELIMINAR_NODO command logic here
    }
    else if (strcmp(buffer, "RECUENTO_NODOS") == 0)
    {
        std::cout << "Received RECUENTO_NODOS command." << std::endl;
        // Implement RECUENTO_NODOS command logic here
    }
    else if (strcmp(buffer, "AÑADIR_ARCHIVO") == 0)
    {
        std::cout << "Received AÑADIR_ARCHIVO command." << std::endl;
        // Implement AÑADIR_ARCHIVO command logic here
    }
    else if (strcmp(buffer, "ELIMINAR_ARCHIVO") == 0)
    {
        std::cout << "Received ELIMINAR_ARCHIVO command." << std::endl;
        // Implement ELIMINAR_ARCHIVO command logic here
    }
    else if (strcmp(buffer, "BUSCAR_ARCHIVOS") == 0)
    {
        std::cout << "Received BUSCAR_ARCHIVOS command." << std::endl;
        // Implement BUSCAR_ARCHIVOS command logic here
    }
    else if (strcmp(buffer, "AÑADIR_TRANSFERENCIA") == 0)
    {
        std::cout << "Received AÑADIR_TRANSFERENCIA command." << std::endl;
        // Implement AÑADIR_TRANSFERENCIA command logic here
    }
    else if (strcmp(buffer, "ELIMINAR_TRANSFERENCIA") == 0)
    {
        std::cout << "Received ELIMINAR_TRANSFERENCIA command." << std::endl;
        // Implement ELIMINAR_TRANSFERENCIA command logic here
    }
    else if (strcmp(buffer, "ACTUALIZAR_ARCHIVOS") == 0)
    {
        std::cout << "Received ACTUALIZAR_ARCHIVOS command." << std::endl;
        // Implement ACTUALIZAR_ARCHIVOS command logic here
    }

    closesocket(client_socket);
}
