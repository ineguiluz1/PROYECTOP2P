#ifndef MENU_H
#define MENU_H

#include <stdio.h>
#include <string.h>
#include <libpq-fe.h>


//Funcion para iniciar la aplicacion de administrador
void initGUI(PGconn *conn);

// Funcion para mostrar el menu de inicio de login
void mostrar_menu_login(PGconn *conn);

// Funcion para mostrar un separador
void insertar_separador();

// Funcion para ejecutar la opcion del menu de login
void ejecutar_opcion_inicio( PGconn *conn,int opcion );

// Funcion para introducir las credenciales de login
void intro_credenciales(PGconn *conn, char* nombre_usu, char* pass);

// Funcion para introducir los datos para el registro
void intro_datos_registro();

// Funcion para mostrar el menu principal de la app
void mostrar_menu_ppal(PGconn *conn);

// Funcion para limpiar la consola
void clear_prompt();

// Funcion para ejecutar la opcion del menu principal
void ejecutar_opcion_ppal( PGconn *conn,int opcion );

// Funcion para mostrar el menu de gestion de nodos
void mostrar_menu_nodos();

// Funcion para mostrar el menu de gestion de archivos
void mostrar_menu_archivos(PGconn *conn);

// Funcion para mostrar las transferencias
void mostrar_transferencias();

// Funcion para mostrar el explorador de usuarios
void mostrar_explorador_usuarios();

// Funcion para mostrar el explorador de archivos
void mostrar_explorador_archivos();

// Funcion para mostrar animacion
void mostrar_animacion();

#endif