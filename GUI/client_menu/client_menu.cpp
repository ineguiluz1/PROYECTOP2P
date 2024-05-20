#include <iostream>
#include <vector>
#include <libpq-fe.h>
#include "client_menu.h"
#include "../../Filesystem/files.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include "../../cliente/client.h"
#include <windows.h>

extern "C"
{
#include "../../estructuras/Archivo/Archivo.h"
}

using namespace std;

void initClientGUI(PGconn *conn, SOCKET clientSocket, char buffer[1024])
{
    mostrarMenuLoginRegister(conn, clientSocket, buffer);
}

void mostrarMenuLoginRegister(PGconn *conn, SOCKET clientSocket, char buffer[1024])
{
    int opcion;
    system("cls");
    cout << "MENU APP P2P" << endl
         << "====================" << endl
         << "1. Iniciar sesion" << endl
         << "2. Registrarse" << endl
         << "3. Salir" << endl
         << "Ingrese una opcion: ";
    cin >> opcion;
    switch (opcion)
    {
    case 1:
        sendMessage(clientSocket, "LOGIN");
        system("cls");
        menuLogin(conn, clientSocket, buffer);
        break;
    case 2:
        sendMessage(clientSocket, "REGISTER");
        system("cls");
        menuRegister(conn, clientSocket, buffer);
        break;
    case 3:
        sendMessage(clientSocket, "bye");
        exit(0);
        break;
    default:
        cout << "Opcion no valida" << endl;
        break;
    }
}

void menuLogin(PGconn *conn, SOCKET clientSocket, char buffer[1024])
{
    char *nombreUsuario = new char[50];
    char *contrasena = new char[50];

    cout << "INICIO DE SESION" << endl
         << "====================" << endl;
    cout << "Ingrese su correo electronico: " << endl;
    cin >> nombreUsuario;
    cout << "Ingrese su contrasena: " << endl;
    cin >> contrasena;

    char *usuarioFormateado = new char[100];
    sprintf(usuarioFormateado, "%s,%s", nombreUsuario, contrasena);
    sendMessage(clientSocket, usuarioFormateado);
    delete[] nombreUsuario;
    delete[] contrasena;
    delete[] usuarioFormateado;
    int bytesReceived = recv(clientSocket, buffer, 1024, 0);
    buffer[bytesReceived] = '\0';
    system("cls");
    if (strcmp(buffer, "ok") == 0)
    {
        char *idUsuario_c = new char[50];
        bytesReceived = recv(clientSocket, idUsuario_c, 50, 0);
        idUsuario_c[bytesReceived] = '\0';
        string idUsuario_str(idUsuario_c);
        int idUsuario = stoi(idUsuario_str);
        delete[] idUsuario_c;
        menuOpcionesPrincipales(conn, clientSocket, buffer, idUsuario);
    }
    else
    {
        mostrarMenuLoginRegister(conn, clientSocket, buffer);
    }
}

void menuRegister(PGconn *conn, SOCKET clientSocket, char buffer[1024])
{
    char *nombreUsuario = new char[50];
    char *contrasena = new char[50];
    char *email = new char[100];

    cout << "REGISTRO" << endl
         << "====================" << endl;
    cout << "Ingrese su nombre de usuario: " << endl;
    cin >> nombreUsuario;
    cout << "Ingrese su contrasena: " << endl;
    cin >> contrasena;
    cout << "Ingrese su email: " << endl;
    cin >> email;

    char *usuarioFormateado = new char[300];
    sprintf(usuarioFormateado, "%s,%s,%s", nombreUsuario, contrasena, email);
    sendMessage(clientSocket, usuarioFormateado);
    int bytesReceived = recv(clientSocket, buffer, 1024, 0);
    buffer[bytesReceived] = '\0';
    system("cls");
    if (strcmp(buffer, "ok") == 0)
    {
        char *idUsuario_c = new char[50];
        bytesReceived = recv(clientSocket, idUsuario_c, 50, 0);
        idUsuario_c[bytesReceived] = '\0';
        string idUsuario_str(idUsuario_c);
        int idUsuario = stoi(idUsuario_str);
        cout << "Usuario: " << idUsuario << endl;
        menuOpcionesPrincipales(conn, clientSocket, buffer, idUsuario);
    }
    else
    {
        mostrarMenuLoginRegister(conn, clientSocket, buffer);
    }
}

void obtenerDatosCarpeta(SOCKET clientSocket, string rutaCarpeta, int idUsuario)
{
    sendMessage(clientSocket, "DATOS_CARPETA");
    vector<string> nombresArchivos = nombreArchivosDeUnaCarpeta(rutaCarpeta);
    vector<int> tamanyoArchivos = tamanyoArchivosDeUnVector(nombresArchivos, rutaCarpeta);
    vector<string> extensionesArchivos = obtenerExtensionDeArchivos(nombresArchivos);
    // cout << "Detalles de archivos obtenidos." << endl;
    int tamanyoArchvios = nombresArchivos.size();
    string tamanyoArchivos_str = to_string(tamanyoArchvios);
    sendMessage(clientSocket, tamanyoArchivos_str.c_str());
    char respuesta[1024] = {0};
    for (int i = 0; i < nombresArchivos.size(); i++)
    {
        int bytesReceived = recv(clientSocket, respuesta, sizeof(respuesta), 0);
        respuesta[bytesReceived] = '\0';
        // cout << "Recibido: " << respuesta << endl;
        if (strcmp(respuesta, "ok") != 0)
        {
            // cout << "Error al enviar los datos de la carpeta" << endl;
            return;
        }
        string infoArchivo = "";
        infoArchivo += rutaCarpeta;
        infoArchivo += "/";
        infoArchivo += nombresArchivos[i];
        infoArchivo += ";";
        infoArchivo += to_string(tamanyoArchivos[i]);
        infoArchivo += ";";
        infoArchivo += extensionesArchivos[i];
        cout << "Enviando: " << infoArchivo << endl;
        sendMessage(clientSocket, infoArchivo.c_str());
    }
    // cout << "Datos de la carpeta enviados correctamente" << endl;
}

void menuSeleccionCarpetaParaCompartir(PGconn *conn, SOCKET clientSocket, char buffer[1024], int idUsuario)
{
    char *rutaAbsolutaACarpeta = new char[500];

    cout << "Ingrese la ruta absoluta de la carpeta que quiere compartir, recuerda sustituir los \\ por /: ";
    cin >> rutaAbsolutaACarpeta;

    obtenerDatosCarpeta(clientSocket, rutaAbsolutaACarpeta, idUsuario);
    menuOpcionesPrincipales(conn, clientSocket, buffer, idUsuario);
}

void menuOpcionesPrincipales(PGconn *conn, SOCKET clientSocket, char buffer[1024], int idUsuario)
{
    int opcion;
    cout << "MENU PRINCIPAL" << endl
         << "====================" << endl
         << "1. Ver archivos disponibles" << endl
         << "2. Buscar archivos por nombre" << endl
         << "3. Descargar archivo" << endl
         << "4. Compartir archivos" << endl
         << "5. Salir" << endl
         << "Ingrese una opcion: ";
    cin >> opcion;
    switch (opcion)
    {
    case 1:
        sendMessage(clientSocket, "consultar_disponibles");
        // cout << "Enviando: consultar_disponibles" << endl;
        mostrarArchivosDisponibles(conn, clientSocket, buffer);
        menuOpcionesPrincipales(conn, clientSocket, buffer, idUsuario);
        break;
    case 2:
        sendMessage(clientSocket, "BUSCAR_ARCHIVO_POR_NOMBRE");
        menuBuscarArchivosPorNombre(conn, clientSocket, buffer);
        break;
    case 3:
        sendMessage(clientSocket, "DESCARGAR_ARCHIVO");
        menuSeleccionArchivoParaDescarga(conn, clientSocket, buffer);
        menuOpcionesPrincipales(conn, clientSocket, buffer, idUsuario);
        break;
    case 4:
        menuCompartirArchivos(conn, clientSocket, buffer, idUsuario);
        break;
    case 5:
    {
        const char *mens = "bye";
        // sendMessage(clientSocket,mensaje);
        sendMessage(clientSocket, mens);
        // cout << "Enviando: " << mens << endl;
        // exit(0);
        break;
    }
    default:
        // cout << "Opcion no valida" << endl;
        menuOpcionesPrincipales(conn, clientSocket, buffer, idUsuario);
        break;
    }
}

void menuBuscarArchivosPorNombre(PGconn *conn, SOCKET clientSocket, char buffer[1024])
{
    char *nombreArchivo = new char[100];
    cout << "BUSCAR ARCHIVOS" << endl
         << "====================" << endl;
    cout << "Ingrese el nombre del archivo que desea buscar: ";
    cin >> nombreArchivo;

    sendMessage(clientSocket, nombreArchivo);
    delete[] nombreArchivo;
    system("cls");
    mostrarArchivosPorElNombre(conn, clientSocket, buffer);
}

void mostrarArchivosPorElNombre(PGconn *conn, SOCKET clientSocket, char buffer[1024])
{
    int bytesReceived = recv(clientSocket, buffer, 1024, 0);
    buffer[bytesReceived] = '\0';
    int cantidadArchivos = atoi(buffer);
    Archivo *archivos = new Archivo[cantidadArchivos];
    cout << "Archivos encontrados" << endl
         << "====================" << endl;
    for (int i = 0; i < cantidadArchivos; i++)
    {
        bytesReceived = recv(clientSocket, buffer, 1024, 0);
        buffer[bytesReceived] = '\0';
        int id = atoi(strtok(buffer, ","));
        char *nombre = strtok(NULL, ",");
        int tamanyo = atoi(strtok(NULL, ","));
        char *tipo = strtok(NULL, ",");
        int id_usuario = atoi(strtok(NULL, ","));

        cout << "Nombre:" << nombre << endl
             << "Tamanyo: " << tamanyo << "bytes" << endl
             << "Tipo: " << tipo << endl
             << "====================" << endl;
    }
    menuOpcionesPrincipales(conn, clientSocket, buffer, 0);
}

void mostrarArchivosDisponibles(PGconn *conn, SOCKET clientSocket, char buffer[1024])
{
    system("cls");
    int bytesReceived = recv(clientSocket, buffer, 1024, 0);
    buffer[bytesReceived] = '\0';
    int cantidadArchivos = atoi(buffer);
    cout << "Cantidad de archivos: " << cantidadArchivos << endl;
    if (cantidadArchivos == 0)
    {
        // cout << "No hay archivos disponibles" << endl;
        return;
    }
    Archivo *archivos = new Archivo[cantidadArchivos];
    cout << "Archivos disponibles" << endl
         << "====================" << endl;
    for (int i = 0; i < cantidadArchivos; i++)
    {
        send(clientSocket, "ok", 2, 0);
        bytesReceived = recv(clientSocket, buffer, 1024, 0);
        buffer[bytesReceived] = '\0';
        int id = atoi(strtok(buffer, ";"));
        char *nombre = strtok(NULL, ";");
        int tamanyo = atoi(strtok(NULL, ";"));
        char *tipo = strtok(NULL, ";");
        int id_usuario = atoi(strtok(NULL, ";"));
        cout << i + 1 << "." << nombre << endl;
    }
}

void menuSeleccionArchivoParaDescarga(PGconn *conn, SOCKET clientSocket, char *buffer)
{
    int opcion;
    mostrarArchivosDisponibles(conn, clientSocket, buffer);

    cout << "Seleccione el archivo que desea descargar: ";
    cin >> opcion;
    cout << "Enviado opcion: " << opcion << endl;
    string opcionStr = to_string(opcion);
    send(clientSocket, opcionStr.c_str(), opcionStr.length(), 0);
    // send(clientSocket, opcion, strlen(opcion), 0);
    int bytesReceived = recv(clientSocket, buffer, 1024, 0);
    buffer[bytesReceived] = '\0';
    string ip_duenyo_archivo(buffer);
    // strcpy(ip_duenyo_archivo, buffer);

    bytesReceived = recv(clientSocket, buffer, 1024, 0);
    buffer[bytesReceived] = '\0';
    string rutaDescarga(buffer);
    // cout << "Ruta descarga: " << rutaDescarga << endl;

    // todo: Si este bloque no funciona, comentarlo
    string rutaGuardado;
    cout << "Elige la ruta donde se guardara el archivo (incluye tambien el nombre del archivo y su extension), recuerda sustituir las \\ por /:" << endl;
    cin >> rutaGuardado;
    //=======================================================================================================

    SOCKET clientSocketTransferencia;
    int bytesReceivedTransferencia;
    conectarConCliente(clientSocketTransferencia, ip_duenyo_archivo.c_str(), 55561);

    send(clientSocketTransferencia, "ruta_archivo", strlen("ruta_archivo"), 0);
    bytesReceivedTransferencia = recv(clientSocketTransferencia, buffer, 1024, 0);
    buffer[bytesReceivedTransferencia] = '\0';
    string confirmacion(buffer);
    if (confirmacion != "ok")
    {
        // cout << "Error al enviar la ruta de archivo" << endl;
        return;
    }
    // cout << "Enviando ruta de archivo..." << endl;
    send(clientSocketTransferencia, rutaDescarga.c_str(), rutaDescarga.length(), 0);
    bytesReceivedTransferencia = recv(clientSocketTransferencia, buffer, 1024, 0);
    buffer[bytesReceivedTransferencia] = '\0';
    string respuesta(buffer);
    if (respuesta != "ok")
    {
        return;
    }
    receiveFile(clientSocketTransferencia, rutaGuardado.c_str(), buffer, sizeof(buffer));

    closesocket(clientSocketTransferencia);
}

void menuCompartirArchivos(PGconn *conn, SOCKET clientSocket, char buffer[1024], int idUsuario)
{
    int opcion;
    cout << "MENU COMPARTIR ARCHIVOS" << endl
         << "====================" << endl
         << "1. Seleccionar carpeta para compartir" << endl
         << "2. Salir" << endl
         << "Ingrese una opcion: ";
    cin >> opcion;
    switch (opcion)
    {
    case 1:
        menuSeleccionCarpetaParaCompartir(conn, clientSocket, buffer, idUsuario);
        break;
    case 2:
        menuOpcionesPrincipales(conn, clientSocket, buffer, idUsuario);
        break;
    default:
        // cout << "Opcion no valida" << endl;
        break;
    }
}