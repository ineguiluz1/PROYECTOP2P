#include <iostream>
#include <libpq-fe.h>
#include "client_menu.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include "../../cliente/client.h"
#include <windows.h>
#include <vector>
extern "C" { // Tell the compiler this is a C function
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
    cout<<"MENU APP P2P"<<endl<<"===================="<<endl<<"1. Iniciar sesion"<<endl<<"2. Registrarse"<<endl<<"3. Salir"<<endl<<"Ingrese una opcion: ";cin>>opcion;
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
        sendMessage(clientSocket, "EXIT");
        exit(0);
        break;
    default:
        cout<<"Opcion no valida"<<endl;
        break;
    }
}

void menuLogin(PGconn *conn, SOCKET clientSocket, char buffer[1024])
{   
    char *nombreUsuario = new char[50];
    char *contrasena = new char[50];

    cout<<"INICIO DE SESION"<<endl<<"===================="<<endl;
    cout<<"Ingrese su correo electronico: "<<endl;cin>>nombreUsuario;
    cout<<"Ingrese su contrasena: "<<endl;cin>>contrasena;

    char *usuarioFormateado = new char[100];
    sprintf(usuarioFormateado, "%s,%s", nombreUsuario, contrasena);
    sendMessage(clientSocket, usuarioFormateado);
    int bytesReceived = recv(clientSocket, buffer, 1024, 0);
    buffer[bytesReceived] = '\0';
    system("cls");
    if (strcmp(buffer, "ok") == 0){
        menuOpcionesPrincipales(conn, clientSocket, buffer);
    }else{
        mostrarMenuLoginRegister(conn, clientSocket, buffer);
    }
}

void menuRegister(PGconn *conn, SOCKET clientSocket, char buffer[1024])
{
    char *nombreUsuario = new char[50];
    char *contrasena = new char[50];
    char *email = new char[100];

    cout<<"REGISTRO"<<endl<<"===================="<<endl;
    cout<<"Ingrese su nombre de usuario: "<<endl;cin>>nombreUsuario;
    cout<<"Ingrese su contrasena: "<<endl;cin>>contrasena;
    cout<<"Ingrese su email: "<<endl;cin>>email;

    char *usuarioFormateado = new char[300];
    sprintf(usuarioFormateado, "%s,%s,%s", nombreUsuario, contrasena, email);
    sendMessage(clientSocket, usuarioFormateado);

    system("cls");
    menuOpcionesPrincipales(conn, clientSocket, buffer);
}

void menuSeleccionCarpetaParaCompartir(PGconn *conn, SOCKET clientSocket, char buffer[1024])
{
    char* rutaAbsolutaACarpeta = new char[500];

    cout<<"Seleccione la carpeta que desea compartir: "<<endl<<"Ingrese la ruta absoluta de la carpeta: ";cin>>rutaAbsolutaACarpeta;
    
    sendMessage(clientSocket, rutaAbsolutaACarpeta);

}

void menuOpcionesPrincipales(PGconn *conn, SOCKET clientSocket, char buffer[1024])
{
    int opcion;
    cout<<"MENU PRINCIPAL"<<endl<<"===================="<<endl<<"1. Ver archivos disponibles"<<endl<<"2. Buscar archivos por nombre"<<endl<<"3. Descargar archivo"<<endl<<"4. Salir"<<endl<<"Ingrese una opcion: ";cin>>opcion;
    switch (opcion)
    {
    case 1:
        sendMessage(clientSocket, "VER_ARCHIVOS_DISPONIBLES");
        break;
    case 2:
        sendMessage(clientSocket, "BUSCAR_ARCHIVO_POR_NOMBRE");
        menuBuscarArchivosPorNombre(conn, clientSocket, buffer);
        break;
    case 3:
        sendMessage(clientSocket, "DESCARGAR_ARCHIVO");
        break;
    case 4:{
        const char *mens = "bye";
        //sendMessage(clientSocket,mensaje);
        sendMessage(clientSocket,mens);
        cout<<"Enviando: "<<mens<<endl;
        //exit(0);
        break;}
    default:
        cout<<"Opcion no valida"<<endl;
        break;
    }
}

void menuBuscarArchivosPorNombre(PGconn *conn, SOCKET clientSocket, char buffer[1024])
{
    char *nombreArchivo = new char[100];
    cout<<"BUSCAR ARCHIVOS"<<endl<<"===================="<<endl;
    cout<<"Ingrese el nombre del archivo que desea buscar: ";cin>>nombreArchivo;

    sendMessage(clientSocket, nombreArchivo);
    system("cls");
}

void menuSeleccionArchivoParaDescarga(PGconn *conn,vector<Archivo> archivos, SOCKET clientSocket, char buffer[1024])
{
    int opcion;
    cout<<"Seleccione el archivo que desea descargar: "<<endl;
    for(int i = 0; i < archivos.size(); i++)
    {
        cout<<i+1<<". "<<archivos[i].nombre<<endl;
    }
    cout<<"Ingrese una opcion: ";cin>>opcion;

    char *archivoSeleccionado = archivos[opcion-1].nombre;
    sendMessage(clientSocket, archivoSeleccionado);
    
}