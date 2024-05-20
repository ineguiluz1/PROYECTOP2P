#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "client.h"
#include <libpq-fe.h>
#include <stdbool.h>
#include <thread>
#include <string>
#include <cstring>
#include <vector>
#include "../GUI/client_menu/client_menu.h"
#include "../networking/servidor/ServidorCliente.h"
#include "../networking/servidor/servidor.h"

extern "C"
{ // Tell the compiler this is a C function
#include "../BD/bd.h"
}

//! gcc -c ../estructuras/Nodo/Nodo.c ../estructuras/Transferencia/Transferencia.c ../BD/bd.c ../estructuras/Archivo/Archivo.c ../estructuras/Usuario/Usuario.c -I "C:\Program Files\PostgreSQL\16\include" -L "C:\Program Files\PostgreSQL\16\lib" -lpq
//! g++ -c client.cpp ../GUI/client_menu/client_menu.cpp ../networking/servidor/servidorCliente.cpp ../networking/servidor/servidor.cpp ../Filesystem/files.cpp -I "C:\Program Files\PostgreSQL\16\include" -L "C:\Program Files\PostgreSQL\16\lib" -lpq -lws2_32
// Compilar el siguiente cliente en el puerto 55560
//! g++ -o client.exe *.o -lstdc++ -I "C:\Program Files\PostgreSQL\16\include" -L "C:\Program Files\PostgreSQL\16\lib" -lpq -lws2_32
// Compilar el siguiente cliente en el puerto 55561
//! g++ -o client2.exe *.o -lstdc++ -I "C:\Program Files\PostgreSQL\16\include" -L "C:\Program Files\PostgreSQL\16\lib" -lpq -lws2_32

void hiloTransferencias()
{
    ServidorCliente servidor(55561); // Cliente 1: 55560, Cliente 2: 55561
    servidor.iniciar();
    std::cout << "Servidor de transferencias iniciado" << std::endl;
    servidor.aceptarConexiones();
}

int main()
{
    std::thread t1(hiloTransferencias);
    t1.detach();

    char buffer[1024] = {0};

    // Initialize Winsock
    if (initClient() != 0)
    {
        std::cerr << "Failed to initialize Winsock." << std::endl;
        return 1;
    }

    PGconn *conn;
    bool estadoConexionBD = conexionBD(&conn);
    if (!estadoConexionBD)
    {
        std::cerr << "Error al conectar con la base de datos" << std::endl;
        exit(1);
    }

    // Create a socket
    SOCKET clientSocket;
    if (socketCreation(clientSocket) != 0)
    {
        std::cerr << "Failed to create socket." << std::endl;
        return 1;
    }

    // Connect to the server
    if (connectionToServer(clientSocket, "127.0.0.1", 55555) != 0)
    {
        std::cerr << "Failed to connect to the server." << std::endl;
        return 1;
    }

    initClientGUI(conn, clientSocket, buffer);

    // Cleanup (close client socket, etc.) can be handled elsewhere in the code
    closesocket(clientSocket);
    WSACleanup();

    return 0;
};

// Function definitions
int initClient()
{
    // Initialize WSA variables
    WSADATA wsaData;
    int wserr;
    WORD wVersionRequested = MAKEWORD(2, 2);
    wserr = WSAStartup(wVersionRequested, &wsaData);

    // Check for initialization success
    if (wserr != 0)
    {
        std::cout << "The winsock dll not found" << std::endl;
        return 1;
    }
    else
    {
        std::cout << "The Winsock dll found" << std::endl;
        std::cout << "The status: " << wsaData.szSystemStatus << std::endl;
    }
    // Continue with the client setup...

    // (See the full code here: https://github.com/Tharun8951/cpp-tcp-server/")

    return 0;
}

int socketCreation(SOCKET &clientSocket)
{
    // Create a socket
    clientSocket = INVALID_SOCKET;
    clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    // Check for socket creation success
    if (clientSocket == INVALID_SOCKET)
    {
        std::cout << "Error at socket(): " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }
    else
    {
        std::cout << "Socket is OK!" << std::endl;
    }
    return 0;
}

int connectionToServer(SOCKET &clientSocket, const char *ip, int port)
{
    // Connect to the server
    sockaddr_in clientService;
    clientService.sin_family = AF_INET;
    clientService.sin_addr.s_addr = inet_addr(ip); // Replace with the server's IP address
    clientService.sin_port = htons(port);          // Use the same port as the server

    // Use the connect function
    if (connect(clientSocket, reinterpret_cast<SOCKADDR *>(&clientService), sizeof(clientService)) == SOCKET_ERROR)
    {
        std::cout << "Client: connect() - Failed to connect: " << WSAGetLastError() << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }
    else
    {
        std::cout << "Client: Connect() is OK!" << std::endl;
        std::cout << "Client: Can start sending and receiving data..." << std::endl;
    }
    return 0;
}

void sendMessage(SOCKET &clientSocket, const char *message)
{
    send(clientSocket, message, strlen(message), 0);
}

void conectarConCliente(SOCKET &clientSocketTransferencia, const char *ip, int port)
{
    if (initClient() != 0)
    {
        std::cerr << "Failed to initialize Winsock." << std::endl;
        return;
    }

    if (socketCreation(clientSocketTransferencia) != 0)
    {
        std::cerr << "Failed to create socket." << std::endl;
        return;
    }
    // todo: descomentar linea de abajo y borrar la siguiente (esta comentada para hacer pruebas
    //  if (connectionToServer(clientSocketTransferencia,ip_duenyo_archivo,55560) != 0)
    if (connectionToServer(clientSocketTransferencia, ip, port) != 0)
    {
        std::cerr << "Failed to connect to the server." << std::endl;
        return;
    }
}

void receiveFile(SOCKET &clientSocket, const char *fileName, char *buffer, size_t bufferSize)
{
    FILE *file;
    std::string path(fileName);

    // Open the file in text or binary mode
    if (path.substr(path.find_last_of(".") + 1) == "txt")
    {
        file = fopen(fileName, "w");
    }
    else
    {
        file = fopen(fileName, "wb");
    }

    // Check if file was opened successfully
    if (file == NULL)
    {
        std::cerr << "Error opening file: " << fileName << std::endl;
        return;
    }

    // Receive the file
    int bytesRead;
    while ((bytesRead = recv(clientSocket, buffer, bufferSize, 0)) > 0)
    {
        if (strcmp(buffer, "end") == 0)
        {
            break;
        }
        fwrite(buffer, 1, bytesRead, file);
    }
    std::cout << "Transferencia recibida" << std::endl;
    if (bytesRead == SOCKET_ERROR)
    {
        std::cerr << "Error receiving data: " << WSAGetLastError() << std::endl;
    }

    // Close the file
    fclose(file);
}