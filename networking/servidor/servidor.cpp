#include "Servidor.h"
#include <iostream>
#include <ws2tcpip.h>
#include <thread>
#include <string>
#include <vector>
#include <functional>
#include <cstring>
#include <sstream>
#include <libpq-fe.h>


extern "C" {
    #include "../../BD/bd.h"
}

// Constructor
Servidor::Servidor(int puerto) : puerto(puerto), socketServidor(INVALID_SOCKET) {
    // Inicializar la dirección del servidor
    memset(&direccionServidor, 0, sizeof(direccionServidor));
    direccionServidor.sin_family = AF_INET;
    direccionServidor.sin_addr.s_addr = INADDR_ANY;
    direccionServidor.sin_port = htons(puerto);
}

// Destructor
Servidor::~Servidor() {
    cerrarServidor();
    WSACleanup();
}

// Método para iniciar el servidor
bool Servidor::iniciar() {
    // Inicializar Winsock
    WSADATA wsaData;
    int resultado = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (resultado != 0) {
        std::cerr << "Error al inicializar Winsock: " << resultado << std::endl;
        return false;
    }
    socketServidor = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (socketServidor == INVALID_SOCKET) {
        std::cerr << "Error al crear el socket: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return false;
    }
    // Enlazar el socket a la dirección y puerto
    resultado = bind(socketServidor, (struct sockaddr*)&direccionServidor, sizeof(direccionServidor));
    if (resultado == SOCKET_ERROR) {
        std::cerr << "Error al enlazar el socket: " << WSAGetLastError() << std::endl;
        cerrarServidor();
        return false;
    }
    // Poner el socket en modo escucha
    resultado = listen(socketServidor, SOMAXCONN);
    if (resultado == SOCKET_ERROR) {
        std::cerr << "Error al poner el socket en modo escucha: " << WSAGetLastError() << std::endl;
        cerrarServidor();
        return false;
    }

    std::cout << "Servidor iniciado en el puerto " << puerto << std::endl;
    return true;
}

// Método para aceptar conexiones
void Servidor::aceptarConexiones() {
    sockaddr_in client_addr;
    int clientAddrLen = sizeof(client_addr);
    while (true) {
        std::cout << "Esperando conexiones..." << std::endl;

        // Aceptar una nueva conexión
        SOCKET socketCliente = accept(socketServidor, (SOCKADDR *)&client_addr, &clientAddrLen);
        if (socketCliente == INVALID_SOCKET) {
            std::cerr << "Error al aceptar la conexión: " << WSAGetLastError() << std::endl;
            continue;
        }
        std::cout << "Cliente conectado." << std::endl;
        std::stringstream ss;
        ss << inet_ntoa(client_addr.sin_addr);
        std::string clientIP = ss.str();
        std::cout << "Client IP address: " << clientIP << std::endl;
        char *clientIP_c = new char[clientIP.length() + 1];
        strcpy(clientIP_c, clientIP.c_str());

        //todo: implementar thread

        // manejarCliente(socketCliente,clientIP_c);
        std::thread clientThread(&Servidor::manejarCliente, this, socketCliente, clientIP_c);
        clientThread.detach();
    }
}

// Método para manejar la conexión con un cliente
// void Servidor::manejarCliente(SOCKET socketCliente) {
//     char buffer[1024];
//     int bytesRecibidos;

//     while ((bytesRecibidos = recv(socketCliente, buffer, sizeof(buffer), 0)) > 0) {
//         buffer[bytesRecibidos] = '\0';
//         std::cout << "Mensaje recibido: " << buffer << std::endl;
//         send(socketCliente, buffer, bytesRecibidos, 0); // Echo
//     }

//     if (bytesRecibidos == SOCKET_ERROR) {
//         std::cerr << "Error al recibir datos del cliente: " << WSAGetLastError() << std::endl;
//     }

//     std::cout << "Cliente desconectado." << std::endl;
//     closesocket(socketCliente);
// }

// Método para cerrar el socket del servidor
void Servidor::cerrarServidor() {
    if (socketServidor != INVALID_SOCKET) {
        closesocket(socketServidor);
        socketServidor = INVALID_SOCKET;
    }
}

// int Servidor::receiveMessage(SOCKET clientSocket, char* buffer) {
//     int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
//     if (bytesReceived >= 0) {
//         buffer[bytesReceived] = '\0';
//     } else {
//         buffer[0] = '\0'; // En caso de error, establecer buffer vacío
//     }
//     std::cout << "Mensaje recibido (Func): " << buffer << std::endl;
//     return bytesReceived;
// }

// void Servidor::inicializarBD(PGconn *conn) {
//     if (!conexionBD(&conn))
//     {
//         std::cerr << "Error al conectar con la base de datos" << std::endl;
//     }
// }
