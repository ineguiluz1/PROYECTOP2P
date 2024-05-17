#pragma once
#include <libpq-fe.h>
#include <winsock2.h>
#include <ws2tcpip.h>

void initClientGUI(PGconn* conn, SOCKET clientSocket, char buffer[1024]);

void mostrarMenuLoginRegister(PGconn* conn, SOCKET clientSocket, char buffer[1024]);

void menuLogin(PGconn *conn, SOCKET clientSocket, char buffer[1024]);

void menuRegister(PGconn *conn, SOCKET clientSocket, char buffer[1024]);

void menuOpcionesPrincipales(PGconn *conn, SOCKET clientSocket, char buffer[1024],int idUsuario);

void menuSeleccionCarpetaParaCompartir(PGconn *conn, SOCKET clientSocket, char buffer[1024]);

void menuBuscarArchivosPorNombre(PGconn *conn, SOCKET clientSocket, char buffer[1024]);

void menuCompartirArchivos(PGconn *conn, SOCKET clientSocket, char buffer[1024], int idUsuario);

void mostrarArchivosPorElNombre(PGconn *conn, SOCKET clientSocket, char buffer[1024]);

void mostrarArchivosDisponibles(PGconn *conn, SOCKET clientSocket, char buffer[1024]);

void menuSeleccionArchivoParaDescarga(PGconn *conn, SOCKET clientSocket, char buffer[1024]);

void menuCompartirArchivos(PGconn *conn, SOCKET clientSocket, char buffer[1024], int idUsuario);