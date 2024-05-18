#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <sstream>
#include <thread>
#include <vector>
#include <atomic>
#include <cstring> // Para strlen y strcmp
#include "../archivo_class/archivo_class.h"

extern "C" {
#include "../BD/bd.h"
#include "../estructuras/Archivo/Archivo.h"
}

//! Para compilar : gcc -c ../estructuras/Nodo/Nodo.c ../estructuras/Transferencia/Transferencia.c ../BD/bd.c ../estructuras/Archivo/Archivo.c ../estructuras/Usuario/Usuario.c -I "C:\Program Files\PostgreSQL\16\include" -L "C:\Program Files\PostgreSQL\16\lib" -lpq
//! g++ -c server2.cpp -I "C:\Program Files\PostgreSQL\16\include" -L "C:\Program Files\PostgreSQL\16\lib" -lpq -lws2_32
//! g++ -o server.exe *.o -lstdc++ -I "C:\Program Files\PostgreSQL\16\include" -L "C:\Program Files\PostgreSQL\16\lib" -lpq -lws2_32

using namespace std;

int initServer()
{
    WSADATA wsaData;
    int wsaerr;
    WORD wVersionRequested = MAKEWORD(2, 2);
    wsaerr = WSAStartup(wVersionRequested, &wsaData);

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
    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

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
    sockaddr_in service;
    service.sin_family = AF_INET;
    service.sin_addr.s_addr = inet_addr("127.0.0.1");
    service.sin_port = htons(55555);

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

void handleClient(SOCKET clientSocket, PGconn *conn, char *clientIP, int clientNumber)
{
    std::cout << "Client number: " << clientNumber << std::endl;
    char buffer[1024] = {0};
    int id_usuario = -1;

    while (true)
    {
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);

        if (bytesReceived == SOCKET_ERROR)
        {
            std::cerr << "Error al recibir datos del cliente" << std::endl;
            break;
        }
        else if (bytesReceived == 0)
        {
            std::cerr << "El cliente ha cerrado la conexión" << std::endl;
            break;
        }

        buffer[bytesReceived] = '\0';
        std::cout << "Mensaje recibido: " << buffer << std::endl;

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
            cout << "Dentro de consultar_disponibles" << endl;
            int cantidadArchivos;
            Archivo_descarga *archivos = get_archivos_disponibles_descarga(conn, &cantidadArchivos);
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
                infoArchivo += ";";
                infoArchivo += archivos[i].ip_dir;
                send(clientSocket, infoArchivo.c_str(), strlen(infoArchivo.c_str()), 0);
                cout<<"Archivo enviado: "<<infoArchivo<<endl;
            }
            cout<<"Archivos enviados"<<endl;
            int posicionArchivoElegido;
            bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
            buffer[bytesReceived] = '\0';

            posicionArchivoElegido = atoi(buffer);
            Archivo_descarga archivoElegido = archivos[posicionArchivoElegido-1];
            char ip_duenyo_archivo[20];
            
            strcpy(ip_duenyo_archivo, archivoElegido.ip_dir);

            cout << "IP del dueño del archivo: " << ip_duenyo_archivo << endl;

            send(clientSocket, ip_duenyo_archivo, strlen(ip_duenyo_archivo), 0);
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

    nodo_offline(conn, clientIP, id_usuario);
    closesocket(clientSocket);
}

int connectionsManagement(SOCKET &serverSocket, PGconn *conn)
{
    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        std::cerr << "listen(): Error listening on socket: " << WSAGetLastError() << std::endl;
        return 1;
    }
    else
    {
        std::cout << "listen() is OK! I'm waiting for new connections..." << std::endl;
    }

    sockaddr_in client_addr;
    int clientAddrLen = sizeof(client_addr);
    std::atomic<int> clientNumber{0};
    std::vector<std::thread> threads;

    while (true)
    {
        SOCKET clientSocket = accept(serverSocket, (SOCKADDR *)&client_addr, &clientAddrLen);
        if (clientSocket == INVALID_SOCKET)
        {
            std::cerr << "accept failed: " << WSAGetLastError() << std::endl;
            break;
        }

        std::stringstream ss;
        ss << inet_ntoa(client_addr.sin_addr);
        std::string clientIP = ss.str();
        std::cout << "Client IP address: " << clientIP << std::endl;

        char *clientIP_c = new char[clientIP.length() + 1];
        strcpy(clientIP_c, clientIP.c_str());
        thread clientThread(handleClient, clientSocket, conn, clientIP_c, clientNumber++);
        clientThread.detach(); // Desprender el hilo para que maneje la conexión independientemente
        // threads.emplace_back(handleClient, clientSocket, conn, clientIP_c, clientNumber++);
    }

    // for (auto &thread : threads)
    // {
    //     if (thread.joinable())
    //     {
    //         thread.join();
    //     }
    // }

    closesocket(serverSocket);
    WSACleanup();
    return 0;
}

int main()
{
    if (initServer() != 0)
    {
        std::cerr << "Failed to initialize Winsock." << std::endl;
        return 1;
    }

    PGconn *conn;
    if (!conexionBD(&conn))
    {
        std::cerr << "Error al conectar con la base de datos" << std::endl;
        return 1;
    }

    SOCKET serverSocket;
    if (socketCreation(serverSocket) != 0)
    {
        std::cerr << "Failed to create socket." << std::endl;
        return 1;
    }

    if (bindSocket(serverSocket) != 0)
    {
        std::cerr << "Failed to bind socket." << std::endl;
        return 1;
    }

    if (connectionsManagement(serverSocket, conn) != 0)
    {
        std::cerr << "Failed to manage connections." << std::endl;
        return 1;
    }

    std::cout << "Server setup complete. Waiting for incoming connections..." << std::endl;
    closesocket(serverSocket);
    WSACleanup();

    return 0;
}
