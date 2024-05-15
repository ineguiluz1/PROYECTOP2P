#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <sstream>
#include "server.h"
#include <time.h>
#include <libpq-fe.h>
#include <stdbool.h>
extern "C" { // Tell the compiler this is a C function

#include "../BD/bd.h"
#include "../estructuras/Archivo/Archivo.h"

}

//! Para compilar : gcc -c ../estructuras/Nodo/Nodo.c ../estructuras/Transferencia/Transferencia.c ../BD/bd.c ../estructuras/Archivo/Archivo.c ../estructuras/Usuario/Usuario.c -I "C:\Program Files\PostgreSQL\16\include" -L "C:\Program Files\PostgreSQL\16\lib" -lpq
//! g++ -c server.cpp -I "C:\Program Files\PostgreSQL\16\include" -L "C:\Program Files\PostgreSQL\16\lib" -lpq -lws2_32
//! g++ -o server.exe *.o -lstdc++ -I "C:\Program Files\PostgreSQL\16\include" -L "C:\Program Files\PostgreSQL\16\lib" -lpq -lws2_32

using namespace std;

int main() {
    // Initialize Winsock
    if (initServer() != 0) {
        std::cerr << "Failed to initialize Winsock." << std::endl;
        return 1;
    }

    //Initialize the database
    PGconn *conn;
    bool estadoConexionBD = conexionBD(&conn);
    if(!estadoConexionBD){
        cout << "Error al conectar con la base de datos" << endl;
        exit(1);
    }

    // Create a socket
    SOCKET serverSocket;
    if (socketCreation(serverSocket) != 0) {
        std::cerr << "Failed to create socket." << std::endl;
        return 1;
    }

    // Bind the socket
    if (bindSocket(serverSocket) != 0) {
        std::cerr << "Failed to bind socket." << std::endl;
        return 1;
    }

    SOCKADDR_IN client_addr;

    // Listen and accept connections
    if (connectionsManagement(serverSocket,client_addr, conn) != 0) {
        std::cerr << "Failed to manage connections." << std::endl;
        return 1;
    }

    // Server setup complete
    std::cout << "Server setup complete. Waiting for incoming connections..." << std::endl;

    // Close the server socket (optional, you can handle this differently in a real server application)
    closesocket(serverSocket);
    WSACleanup();

    return 0;
}

int initServer() {
    WSADATA wsaData;
    int wsaerr;
    WORD wVersionRequested = MAKEWORD(2, 2);
    wsaerr = WSAStartup(wVersionRequested, &wsaData);

    // Check for initialization success
    if (wsaerr != 0) {
        std::cerr << "The Winsock dll not found!" << std::endl;
        return 1;
    } else {
        std::cout << "The Winsock dll found" << std::endl;
        std::cout << "The status: " << wsaData.szSystemStatus << std::endl;
    }

    return 0;
}

int socketCreation(SOCKET& serverSocket) {
    // Create a socket
    serverSocket = INVALID_SOCKET;
    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    // Check for socket creation success
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Error at socket(): " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    } else {
        std::cout << "Socket is OK!" << std::endl;
    }

    return 0;
}

int bindSocket(SOCKET& serverSocket) {
    // Bind the socket to an IP address and port number
    sockaddr_in service;
    service.sin_family = AF_INET;
    service.sin_addr.s_addr = inet_addr("127.0.0.1");  // Replace with your desired IP address
    service.sin_port = htons(55555);  // Choose a port number

    // Use the bind function
    if (bind(serverSocket, reinterpret_cast<SOCKADDR*>(&service), sizeof(service)) == SOCKET_ERROR) {
        std::cerr << "bind() failed: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    } else {
        std::cout << "bind() is OK!" << std::endl;
    }

    return 0;
}

void handleClient(SOCKET& clientSocket, PGconn *conn) {
    char buffer[1024] = {0};
    
    // Recibir datos del cliente
    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesReceived == SOCKET_ERROR) {
        std::cerr << "Error al recibir datos del cliente" << std::endl;
    } else if (bytesReceived == 0) {
        std::cerr << "El cliente ha cerrado la conexiÃ³n" << std::endl;
    } else {
        //TODO: Implementar la logica del servidor
        // Analizar el mensaje recibido y responder apropiadamente 
        if (strcmp(buffer, "hola") == 0) {
            const char *response = "Hola cliente!";
            send(clientSocket, response, strlen(response), 0);
        } else if(strcmp(buffer, "LOGIN") == 0) {
            bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
            buffer[bytesReceived] = '\0';

            char *correo = strtok(buffer, ",");
            char *contrasena = strtok(nullptr, ",");

            if(autenticar_usuario(conn, correo, contrasena))
            {
                printf("Usuario autenticado\n");
                
            };
        } else if(strcmp(buffer, "LOGOUT") == 0) {
            const char *response = "LOGOUT";
            cout << "Enviando respuesta: " << response << endl;
            send(clientSocket, response, strlen(response), 0);
        } else if(strcmp(buffer, "REGISTER") == 0) {
            bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
            buffer[bytesReceived] = '\0';

            char *nombre = strtok(buffer, ",");
            printf("Nombre: %s\n", nombre);
            char *contrasena = strtok(nullptr, ",");
            printf("Contrasena: %s\n", contrasena);
            char *correo = strtok(nullptr, ",");
            printf("Correo: %s\n", correo);

            registrar_usuario2(conn, nombre, correo, contrasena);
            const char *response = "REGISTER";
            cout << "Enviando respuesta: " << response << endl;
            send(clientSocket, response, strlen(response), 0);
        } else if(strcmp(buffer, "ENVIAR_LISTA_ARCHIVOS") == 0){
            const char *response = "LISTA_ARCHIVOS";
            cout << "Enviando respuesta: " << response << endl;
            send(clientSocket, response, strlen(response), 0);
        } else if(strcmp(buffer, "AÑADIR_ARCHIVO") == 0){
            bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
            buffer[bytesReceived] = '\0';

            Archivo archivo;
            char *token = strtok(buffer, ",");
            archivo.id = stoi(token);
            token = strtok(nullptr, ",");
            strncpy(archivo.nombre, token, sizeof(archivo.nombre) - 1);
            token = strtok(nullptr, ",");
            archivo.tamanyo = std::stol(token);
            token = strtok(nullptr, ",");
            strncpy(archivo.tipo, token, sizeof(archivo.tipo) - 1);
            token = strtok(nullptr, ",");
            archivo.fecha_subida = std::stol(token);
            token = strtok(nullptr, ",");
            archivo.id_usuario = std::stoi(token);

            insertar_Archivo2(conn, archivo.nombre, archivo.tamanyo, archivo.tipo, archivo.id_usuario);
            
        } else if(strcmp(buffer, "ELIMINAR_ARCHIVO") == 0){
            const char *response = "ELIMINAR_ARCHIVO";
            cout << "Enviando respuesta: " << response << endl;
            send(clientSocket, response, strlen(response), 0);
        } else if(strcmp(buffer, "BUSCAR_ARCHIVO_POR_NOMBRE") == 0){
            bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
            buffer[bytesReceived] = '\0';

            char *nombre;
            strcpy(nombre, buffer);
            printf("Nombre del archivo a buscar: %s\n", nombre);

        } else if(strcmp(buffer, "DESCARGAR_ARCHIVO") == 0){
            bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
            buffer[bytesReceived] = '\0';

            printf("Nombre del archivo a descargar: %s\n", buffer);
        }
        else {
            const char *response = "Comando no reconocido";
            cout << "Enviando respuesta: " << buffer << endl;
            send(clientSocket, response, strlen(response), 0);
        }
    }
}

int connectionsManagement(SOCKET& serverSocket,SOCKADDR_IN& client_addr, PGconn *conn) {
    // Listen for incoming connections
    if (listen(serverSocket, 1) == SOCKET_ERROR) {
        std::cerr << "listen(): Error listening on socket: " << WSAGetLastError() << std::endl;
        return 1;
    } else {
        std::cout << "listen() is OK! I'm waiting for new connections..." << std::endl;
    }

    // **Changes to capture client address information**
    int clientAddrLen = sizeof(client_addr);

    // Accept incoming connections
    SOCKET clientSocket;
    while (true) {
        clientSocket = accept(serverSocket, (SOCKADDR*)&client_addr, &clientAddrLen);
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "accept failed: " << WSAGetLastError() << std::endl;
            break;
        }

        std::cout << "Client IP address: " << client_addr.sin_addr.s_addr << std::endl;
        std::stringstream ss;
        ss << inet_ntoa(client_addr.sin_addr);
        std::string clientIP = ss.str();
        std::cout << "Client IP address: " << clientIP << std::endl;
        // **Access client IP and port (if needed)**
        // You can access the client's IP address using clientAddr.sin_addr.s_addr 
        // and port using clientAddr.sin_port after a successful accept.

        // Print message when connection to client is established
        std::cout << "Connection to a client established." << std::endl;

        // ... (Optional) Handle the connected client (e.g., send/receive data) ...
        // Handle the connected client (e.g., send/receive data)
        while (true) {
            handleClient(clientSocket, conn);
        }
        // Close the client socket
        closesocket(clientSocket);
    }

    std::cout << "Fuera while." << std::endl;
    return 0;
}

int sendAndReceive(SOCKET& clientSocket) {
    // Send and receive data
    char sendbuf[512];
    char recvbuf[512];
    int recvbuflen = 512;
    int iResult;

    // Receive data until the client shuts down the connection
    do {
        iResult = recv(clientSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0) {
            std::cout << "Bytes received: " << iResult << std::endl;
            std::cout << "Received message: " << recvbuf << std::endl;

            // Echo the buffer back to the sender
            iResult = send(clientSocket, recvbuf, iResult, 0);
            if (iResult == SOCKET_ERROR) {
                std::cerr << "send failed: " << WSAGetLastError() << std::endl;
                closesocket(clientSocket);
                WSACleanup();
                return 1;
            }
            std::cout << "Bytes sent: " << iResult << std::endl;
        } else if (iResult == 0) {
            std::cout << "Connection closing..." << std::endl;
        } else {
            std::cerr << "recv failed: " << WSAGetLastError() << std::endl;
            closesocket(clientSocket);
            WSACleanup();
            return 1;
        }
    } while (iResult > 0);

    return 0;
} 
