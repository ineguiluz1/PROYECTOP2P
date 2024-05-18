#include "servidorCliente.h"
#include <iostream>
#include <ws2tcpip.h>
#include <thread>
#include <string>
#include <vector>
#include <functional>
#include <sstream>
#include <cstring>

extern "C"
{ // Tell the compiler this is a C function

#include "../../BD/bd.h"
#include "../../estructuras/Archivo/Archivo.h"
}

using namespace std;

ServidorCliente::ServidorCliente(int puerto) : Servidor(puerto)
{
    // Puedes agregar inicializaciones adicionales si es necesario
}

// Destructor de ServidorIndices
ServidorCliente::~ServidorCliente()
{
    // No se necesita realizar ninguna limpieza adicional en este caso
}

// Método sobrescrito para manejar la conexión con un cliente
void ServidorCliente::manejarCliente(SOCKET socketCliente, char *clientIP)
{
    PGconn *conn;
    if (!conexionBD(&conn))
    {
        std::cerr << "Error al conectar con la base de datos" << std::endl;
        return;
    }

    char buffer[1024] = {0};
    int id_usuario = -1;

    // ServidorIndices::receiveMessage(socketCliente, buffer);
    while (true)
    {
        int bytesReceived = recv(socketCliente, buffer, sizeof(buffer) - 1, 0);
        buffer[bytesReceived] = '\0';
        cout << "Mensaje recibido: " << buffer << endl;
        if (strcmp(buffer, "hola") == 0)
        {
            const char *response = "Hola cliente!";
            send(socketCliente, response, strlen(response), 0);
        }
        else
        {
            const char *response = "Comando no reconocido";
        }
    }

    closesocket(socketCliente);
}
