#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <sstream>
#include "server.h"
#include <time.h>
#include <cstdlib>
#include <libpq-fe.h>
#include <stdbool.h>
extern "C"
{ // Tell the compiler this is a C function

#include "../BD/bd.h"
#include "../estructuras/Archivo/Archivo.h"
}

//! Para compilar : gcc -c ../estructuras/Nodo/Nodo.c ../estructuras/Transferencia/Transferencia.c ../BD/bd.c ../estructuras/Archivo/Archivo.c ../estructuras/Usuario/Usuario.c -I "C:\Program Files\PostgreSQL\16\include" -L "C:\Program Files\PostgreSQL\16\lib" -lpq
//! g++ -c server.cpp -I "C:\Program Files\PostgreSQL\16\include" -L "C:\Program Files\PostgreSQL\16\lib" -lpq -lws2_32
//! g++ -o server.exe *.o -lstdc++ -I "C:\Program Files\PostgreSQL\16\include" -L "C:\Program Files\PostgreSQL\16\lib" -lpq -lws2_32

using namespace std;

int main()
{
    // Initialize Winsock
    if (initServer() != 0)
    {
        std::cerr << "Failed to initialize Winsock." << std::endl;
        return 1;
    }

    // Initialize the database
    PGconn *conn;
    bool estadoConexionBD = conexionBD(&conn);
    if (!estadoConexionBD)
    {
        cout << "Error al conectar con la base de datos" << endl;
        exit(1);
    }

    // Create a socket
    SOCKET serverSocket;
    if (socketCreation(serverSocket) != 0)
    {
        std::cerr << "Failed to create socket." << std::endl;
        return 1;
    }

    // Bind the socket
    if (bindSocket(serverSocket) != 0)
    {
        std::cerr << "Failed to bind socket." << std::endl;
        return 1;
    }

    SOCKADDR_IN client_addr;

    // Listen and accept connections
    if (connectionsManagement(serverSocket, client_addr, conn) != 0)
    {
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

int initServer()
{
    WSADATA wsaData;
    int wsaerr;
    WORD wVersionRequested = MAKEWORD(2, 2);
    wsaerr = WSAStartup(wVersionRequested, &wsaData);

    // Check for initialization success
    if (wsaerr != 0)
    {
        std::cerr << "The Winsock dll not found!" << std::endl;
        return 1;
    }
    else
    {
        std::cout << "The Winsock dll found" << std::endl;
        std::cout << "The status: " << wsaData.szSystemStatus << std::endl;
    }

    return 0;
}

int socketCreation(SOCKET &serverSocket)
{
    // Create a socket
    serverSocket = INVALID_SOCKET;
    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    // Check for socket creation success
    if (serverSocket == INVALID_SOCKET)
    {
        std::cerr << "Error at socket(): " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }
    else
    {
        std::cout << "Socket is OK!" << std::endl;
    }

    return 0;
}

int bindSocket(SOCKET &serverSocket)
{
    // Bind the socket to an IP address and port number
    sockaddr_in service;
    service.sin_family = AF_INET;
    service.sin_addr.s_addr = inet_addr("127.0.0.1"); // Replace with your desired IP address
    service.sin_port = htons(55555);                  // Choose a port number

    // Use the bind function
    if (bind(serverSocket, reinterpret_cast<SOCKADDR *>(&service), sizeof(service)) == SOCKET_ERROR)
    {
        std::cerr << "bind() failed: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }
    else
    {
        std::cout << "bind() is OK!" << std::endl;
    }

    return 0;
}

bool handleClient(SOCKET &clientSocket, PGconn *conn, char *clientIP)
{
    char buffer[1024] = {0};
    int id_usuario = -1;
    while (true)
    {
        // Recibir datos del cliente
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        buffer[bytesReceived] = '\0';
        cout << "Mensaje recibido: " << buffer << endl;
        if (bytesReceived == SOCKET_ERROR)
        {
            std::cerr << "Error al recibir datos del cliente" << std::endl;
            break;
        }
        else if (bytesReceived == 0)
        {
            std::cerr << "El cliente ha cerrado la conexiÃ³n" << std::endl;
            break;
        }
        else
        {
            // TODO: Implementar la logica del servidor
            //  Analizar el mensaje recibido y responder apropiadamente
            if (strcmp(buffer, "hola") == 0)
            {
                const char *response = "Hola cliente!";
                send(clientSocket, response, strlen(response), 0);
            }
            else if (strcmp(buffer, "LOGIN") == 0)
            {
                bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
                buffer[bytesReceived] = '\0';

                char *correo = strtok(buffer, ",");
                char *contrasena = strtok(nullptr, ",");

                if (autenticar_usuario(conn, correo, contrasena))
                {
                    printf("Usuario autenticado\n");
                    send(clientSocket, "ok", 2, 0);
                    id_usuario = get_id_from_username_password(conn, correo, contrasena);
                    string id_str = to_string(id_usuario);
                    const char *id_c_str = id_str.c_str();
                    send(clientSocket, id_c_str, sizeof(id_c_str), 0);
                    nodo_online(conn, clientIP, id_usuario);
                }
                else
                {
                    printf("Usuario no autenticado\n");
                    send(clientSocket, "no", 2, 0);
                };
            }
            else if (strcmp(buffer, "LOGOUT") == 0)
            {
                const char *response = "LOGOUT";
                cout << "Enviando respuesta: " << response << endl;
                send(clientSocket, response, strlen(response), 0);
            }
            else if (strcmp(buffer, "REGISTER") == 0)
            {
                bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
                buffer[bytesReceived] = '\0';

                char *nombre = strtok(buffer, ",");
                printf("Nombre: %s\n", nombre);
                char *contrasena = strtok(nullptr, ",");
                printf("Contrasena: %s\n", contrasena);
                char *correo = strtok(nullptr, ",");
                printf("Correo: %s\n", correo);
                const char *response;

                if (registrar_usuario2(conn, nombre, correo, contrasena))
                {
                    response = "ok";
                    cout << "Enviando respuesta: " << response << endl;
                    send(clientSocket, response, strlen(response), 0);
                    id_usuario = get_id_from_username_password(conn, correo, contrasena);
                    string id_str = to_string(id_usuario);
                    const char *id_c_str = id_str.c_str();
                    send(clientSocket, id_c_str, sizeof(id_c_str), 0);
                }
                else
                {
                    response = "no ok";
                    cout << "Enviando respuesta: " << response << endl;
                    send(clientSocket, response, strlen(response), 0);
                }
            }
            else if (strcmp(buffer, "ENVIAR_LISTA_ARCHIVOS") == 0)
            {
                const char *response = "LISTA_ARCHIVOS";
                cout << "Enviando respuesta: " << response << endl;
                send(clientSocket, response, strlen(response), 0);
            }
            else if (strcmp(buffer, "AÑADIR_ARCHIVO") == 0)
            {
                bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
                buffer[bytesReceived] = '\0';
            }
            else if (strcmp(buffer, "ELIMINAR_ARCHIVO") == 0)
            {
                const char *response = "ELIMINAR_ARCHIVO";
                cout << "Enviando respuesta: " << response << endl;
                send(clientSocket, response, strlen(response), 0);
            }
            else if (strcmp(buffer, "BUSCAR_ARCHIVO_POR_NOMBRE") == 0)
            {

                bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
                buffer[bytesReceived] = '\0';

                int cantidadArchivos;
                Archivo *archivos = busqueda_archivos_nombre(conn, buffer, &cantidadArchivos);

                sprintf(buffer, "%d", cantidadArchivos);
                send(clientSocket, buffer, strlen(buffer), 0);

                for (int i = 0; i < cantidadArchivos; i++)
                {
                    sprintf(buffer, "%d,%s,%ld,%s,%d", archivos[i].id, archivos[i].nombre, archivos[i].tamanyo, archivos[i].tipo, archivos[i].id_usuario);
                    send(clientSocket, buffer, strlen(buffer), 0);
                }
            }
            else if (strcmp(buffer, "DESCARGAR_ARCHIVO") == 0)
            {
                int cantidadArchivos;
                Archivo *archivos = get_archivos_disponibles(conn, &cantidadArchivos);

                sprintf(buffer, "%d", cantidadArchivos);
                send(clientSocket, buffer, strlen(buffer), 0);

                int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
                buffer[bytesReceived] = '\0';
                printf("Nombre del archivo a descargar: %s\n", buffer);

                if (strcmp(buffer, "CANTIDAD_RECIBIDA") == 0)
                {
                    for (int i = 0; i < cantidadArchivos; i++)
                    {
                        sprintf(buffer, "%d,%s,%ld,%s,%d", archivos[i].id, archivos[i].nombre, archivos[i].tamanyo, archivos[i].tipo, archivos[i].id_usuario);
                        send(clientSocket, buffer, strlen(buffer), 0);
                    }
                }
            }
            else if (strcmp(buffer, "consultar_disponibles") == 0)
            {

                cout << "Dentro de consultar_disponibles" << endl;
                int cantidadArchivos;
                Archivo *archivos = get_archivos_disponibles(conn, &cantidadArchivos);
                cout << "Cantidad de archivos: " << cantidadArchivos << endl;
                string cantidadArchivosChar = to_string(cantidadArchivos);
                send(clientSocket, cantidadArchivosChar.c_str(), strlen(cantidadArchivosChar.c_str()), 0);
                char respuesta[1024] = {0};
                for (int i = 0; i < cantidadArchivos; i++)
                {
                    int bytesReceived = recv(clientSocket, respuesta, sizeof(respuesta), 0);
                    respuesta[bytesReceived] = '\0';
                    cout << "Recibido: " << respuesta << endl;
                    if (strcmp(respuesta, "ok") != 0)
                    {
                        cout << "Error al enviar los datos de la carpeta" << endl;
                        break;
                    }
                    string infoArchivo = ""; // String para almacenar la información del archivo
                    infoArchivo += to_string(archivos[i].id);
                    infoArchivo += ";";
                    infoArchivo += archivos[i].nombre;
                    infoArchivo += ";";
                    infoArchivo += to_string(archivos[i].tamanyo);
                    infoArchivo += ";";
                    infoArchivo += archivos[i].tipo;
                    infoArchivo += ";";
                    infoArchivo += to_string(archivos[i].id_usuario);
                    send(clientSocket, infoArchivo.c_str(), strlen(infoArchivo.c_str()), 0);
                }
            }
            else if (strcmp(buffer, "bye") == 0)
            {
                printf("Cerrando conexion con cliente\n");
                break;
            }
            else if (strcmp(buffer, "DATOS_CARPETA") == 0)
            {
                bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
                buffer[bytesReceived] = '\0';
                int lengthArchivos = atoi(buffer);
                cout << "Cantidad de archivos: " << lengthArchivos << endl;
                send(clientSocket, "ok", 2, 0);
                eliminarFilasWhereIdUsuario(conn, id_usuario);
                cout << "Eliminados archivos de la base de datos" << endl;
                for (int i = 0; i < lengthArchivos; i++)
                {
                    bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
                    buffer[bytesReceived] = '\0';
                    cout << "Recibido: " << buffer << endl;
                    std::string nombre, tamanyo, extension;
                    std::stringstream ss(buffer);
                    std::getline(ss, nombre, ';');
                    std::getline(ss, tamanyo, ';');
                    std::getline(ss, extension, ';');
                    char *nombreChar = new char[nombre.length() + 1];
                    strcpy(nombreChar, nombre.c_str());
                    char *tamanyoChar = new char[tamanyo.length() + 1];
                    strcpy(tamanyoChar, tamanyo.c_str());
                    char *extensionChar = new char[extension.length() + 1];
                    strcpy(extensionChar, extension.c_str());
                    insertar_Archivo2(conn, nombreChar, tamanyoChar, extensionChar, id_usuario);
                    send(clientSocket, "ok", 2, 0);
                }
            }
            else
            {
                const char *response = "Comando no reconocido";
            }
        }
    }
    nodo_offline(conn, clientIP, id_usuario);
    return true;
}

int connectionsManagement(SOCKET &serverSocket, SOCKADDR_IN &client_addr, PGconn *conn)
{
    // Listen for incoming connections
    if (listen(serverSocket, 1) == SOCKET_ERROR)
    {
        std::cerr << "listen(): Error listening on socket: " << WSAGetLastError() << std::endl;
        return 1;
    }
    else
    {
        std::cout << "listen() is OK! I'm waiting for new connections..." << std::endl;
    }

    // **Changes to capture client address information**
    int clientAddrLen = sizeof(client_addr);

    // Accept incoming connections
    SOCKET clientSocket;
    while (true)
    {
        clientSocket = accept(serverSocket, (SOCKADDR *)&client_addr, &clientAddrLen);
        if (clientSocket == INVALID_SOCKET)
        {
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
        char *clientIP_c = new char[clientIP.length() + 1];
        strcpy(clientIP_c, clientIP.c_str());
        handleClient(clientSocket, conn, clientIP_c);

        // Close the client socket
        closesocket(clientSocket);
    }

    std::cout << "Fuera while." << std::endl;
    closesocket(serverSocket);
    WSACleanup();
    return 0;
}
