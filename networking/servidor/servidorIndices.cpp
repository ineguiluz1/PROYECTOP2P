#include "ServidorIndices.h"
#include <iostream>
#include <ws2tcpip.h>
#include <thread>
#include <string>
#include <vector>
#include <functional>
#include <sstream>
#include <cstring>
#include "../../FicheroLog/logFile.h"


extern "C"
{ // Tell the compiler this is a C function

#include "../../BD/bd.h"
#include "../../estructuras/Archivo/Archivo.h"
}

using namespace std;

ServidorIndices::ServidorIndices(int puerto) : Servidor(puerto) {
    // Puedes agregar inicializaciones adicionales si es necesario
}

// Destructor de ServidorIndices
ServidorIndices::~ServidorIndices() {
    // No se necesita realizar ninguna limpieza adicional en este caso
}


// Método sobrescrito para manejar la conexión con un cliente
void ServidorIndices::manejarCliente(SOCKET socketCliente, char *clientIP) {
    
    Log log("../FicheroLog/log.txt");
    PGconn *conn;
    if (!conexionBD(&conn))
    {
        std::cerr << "Error al conectar con la base de datos" << std::endl;
        return;
    }
    //Servidor::inicializarBD(conn);

    //std::cout << "Client number: " << clientNumber << std::endl;
    char buffer[1024] = {0};
    int id_usuario = -1;

    //ServidorIndices::receiveMessage(socketCliente, buffer);
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
        else if (strcmp(buffer, "LOGIN") == 0)
        {
            int bytesReceived = recv(socketCliente, buffer, sizeof(buffer) - 1, 0);
            buffer[bytesReceived] = '\0';
            char *correo = strtok(buffer, ",");
            char *contrasena = strtok(nullptr, ",");

            if (autenticar_usuario(conn, correo, contrasena))
            {
                printf("Usuario autenticado\n");
                send(socketCliente, "ok", 2, 0);
                id_usuario = get_id_from_username_password(conn, correo, contrasena);
                string id_str = to_string(id_usuario);
                const char *id_c_str = id_str.c_str();
                send(socketCliente, id_c_str, sizeof(id_c_str), 0);
                nodo_online(conn, clientIP, id_usuario);
                log.writeLog(INFO, "Usuario autenticado");
            }
            else
            {
                printf("Usuario no autenticado\n");
                send(socketCliente, "no", 2, 0);
                log.writeLog(FATAL, "Usuario no autenticado");
            };
        }
        else if (strcmp(buffer, "LOGOUT") == 0)
        {
            const char *response = "LOGOUT";
            cout << "Enviando respuesta: " << response << endl;
            send(socketCliente, response, strlen(response), 0);
        }
        else if (strcmp(buffer, "REGISTER") == 0)
        {
            bytesReceived = recv(socketCliente, buffer, sizeof(buffer), 0);
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
                send(socketCliente, response, strlen(response), 0);
                id_usuario = get_id_from_username_password(conn, correo, contrasena);
                string id_str = to_string(id_usuario);
                const char *id_c_str = id_str.c_str();
                send(socketCliente, id_c_str, sizeof(id_c_str), 0);
                log.writeLog(INFO, "Usuario registrado");
            }
            else
            {
                response = "no ok";
                send(socketCliente, response, strlen(response), 0);
                log.writeLog(FATAL, "Usuario no registrado");
            }
        }
        else if (strcmp(buffer, "BUSCAR_ARCHIVO_POR_NOMBRE") == 0)
        {

            bytesReceived = recv(socketCliente, buffer, sizeof(buffer), 0);
            buffer[bytesReceived] = '\0';

            int cantidadArchivos;
            Archivo *archivos = busqueda_archivos_nombre(conn, buffer, &cantidadArchivos);

            sprintf(buffer, "%d", cantidadArchivos);
            if(cantidadArchivos == 0)
            {
                log.writeLog(INFO, "No se encontraron archivos en la búsqueda por nombre.");
            }else
            {
                log.writeLog(INFO, "Archivos encontrados en la búsqueda por nombre.");
            }
                
            send(socketCliente, buffer, strlen(buffer), 0);

            for (int i = 0; i < cantidadArchivos; i++)
            {
                sprintf(buffer, "%d,%s,%ld,%s,%d", archivos[i].id, archivos[i].nombre, archivos[i].tamanyo, archivos[i].tipo, archivos[i].id_usuario);
                send(socketCliente, buffer, strlen(buffer), 0);
            }
        }
        else if (strcmp(buffer, "DESCARGAR_ARCHIVO") == 0)
        {
            int cantidadArchivos;
            Archivo_descarga *archivos = get_archivos_disponibles_descarga(conn, &cantidadArchivos);
            string cantidadArchivosChar = to_string(cantidadArchivos);
            send(socketCliente, cantidadArchivosChar.c_str(), strlen(cantidadArchivosChar.c_str()), 0);
            char respuesta[1024] = {0};
            for (int i = 0; i < cantidadArchivos; i++)
            {
                int bytesReceived = recv(socketCliente, respuesta, sizeof(respuesta), 0);
                respuesta[bytesReceived] = '\0';
                cout << "Recibido: " << respuesta << endl;
                if (strcmp(respuesta, "ok") != 0)
                {
                    log.writeLog(FATAL, "Error al enviar los datos de la carpeta");
                    break;
                }else
                {
                    log.writeLog(INFO, "Datos de la carpeta enviados correctamente");
                }
                char* path = archivos[i].nombre;            
                char* filename = strrchr(path, '/');
                // Si se encuentra el carácter, avanza un paso para obtener el nombre del archivo
                if (filename != nullptr) {
                    filename++; // Avanza para saltar el '/'
                }
                string infoArchivo = ""; // String para almacenar la información del archivo
                infoArchivo += to_string(archivos[i].id);
                infoArchivo += ";";
                infoArchivo += filename;
                infoArchivo += ";";
                infoArchivo += to_string(archivos[i].tamanyo);
                infoArchivo += ";";
                infoArchivo += archivos[i].tipo;
                infoArchivo += ";";
                infoArchivo += to_string(archivos[i].id_usuario);
                infoArchivo += ";";
                infoArchivo += archivos[i].ip_dir;
                send(socketCliente, infoArchivo.c_str(), strlen(infoArchivo.c_str()), 0);
                //cout<<"Archivo enviado: "<<infoArchivo<<endl;
            }
            int posicionArchivoElegido;
            bytesReceived = recv(socketCliente, buffer, sizeof(buffer), 0);
            buffer[bytesReceived] = '\0';

            cout << "Archivo elegido: " << buffer << endl;

            posicionArchivoElegido = atoi(buffer);
            Archivo_descarga archivoElegido = archivos[posicionArchivoElegido-1];
            char ip_duenyo_archivo[20];
            strcpy(ip_duenyo_archivo, archivoElegido.ip_dir);
            send(socketCliente, ip_duenyo_archivo, strlen(ip_duenyo_archivo), 0);

            char rutaArchivo[1000];
            strcpy(rutaArchivo, archivoElegido.nombre);
            send(socketCliente, rutaArchivo, strlen(rutaArchivo), 0);

        }
        else if (strcmp(buffer, "consultar_disponibles") == 0)
        {

            int cantidadArchivos;
            Archivo *archivos = get_archivos_disponibles(conn, &cantidadArchivos);
            string cantidadArchivosChar = to_string(cantidadArchivos);
            send(socketCliente, cantidadArchivosChar.c_str(), strlen(cantidadArchivosChar.c_str()), 0);
            char respuesta[1024] = {0};
            for (int i = 0; i < cantidadArchivos; i++)
            {
                int bytesReceived = recv(socketCliente, respuesta, sizeof(respuesta), 0);
                respuesta[bytesReceived] = '\0';
                if (strcmp(respuesta, "ok") != 0)
                {
                    log.writeLog(FATAL, "Error al enviar los datos de la carpeta");
                    break;
                }else
                {
                    log.writeLog(INFO, "Datos de la carpeta enviados correctamente");
                }
                char* path = archivos[i].nombre;            
                char* filename = strrchr(path, '/');
                // Si se encuentra el carácter, avanza un paso para obtener el nombre del archivo
                if (filename != nullptr) {
                    filename++; // Avanza para saltar el '/'
                }else{
                    filename = path;
                }
                string infoArchivo = ""; // String para almacenar la información del archivo
                infoArchivo += to_string(archivos[i].id);
                infoArchivo += ";";
                infoArchivo += filename;
                infoArchivo += ";";
                infoArchivo += to_string(archivos[i].tamanyo);
                infoArchivo += ";";
                infoArchivo += archivos[i].tipo;
                infoArchivo += ";";
                infoArchivo += to_string(archivos[i].id_usuario);
                send(socketCliente, infoArchivo.c_str(), strlen(infoArchivo.c_str()), 0);
            }
        }
        else if (strcmp(buffer, "bye") == 0)
        {
            printf("Cerrando conexion con cliente\n");
            log.writeLog(INFO, "Cerrando conexión con el cliente");
            break;
        }
        else if (strcmp(buffer, "DATOS_CARPETA") == 0)
        {
            bytesReceived = recv(socketCliente, buffer, sizeof(buffer), 0);
            buffer[bytesReceived] = '\0';
            int lengthArchivos = atoi(buffer);
            send(socketCliente, "ok", 2, 0);
            eliminarFilasWhereIdUsuario(conn, id_usuario);
            cout << "Eliminados archivos de la base de datos" << endl;
            for (int i = 0; i < lengthArchivos; i++)
            {
                bytesReceived = recv(socketCliente, buffer, sizeof(buffer), 0);
                buffer[bytesReceived] = '\0';
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
                send(socketCliente, "ok", 2, 0);
            }
        }
        else
        {
            // const char *response = "Comando no reconocido";
            cout << "Comando no reconocido" << endl;
            log.writeLog(WARNING, "Comando no reconocido");
            break;
        }
    }

    nodo_offline(conn, clientIP, id_usuario);
    closesocket(socketCliente);

}

