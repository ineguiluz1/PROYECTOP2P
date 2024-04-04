#include <stdio.h>
#include <stdlib.h>
#include <libpq-fe.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include "../estructuras/Archivo/Archivo.h"
#include "../estructuras/Usuario/Usuario.h"
#ifndef BD_H
#define BD_H

// Funcion para conectar a la base de datos
bool conexionBD(PGconn **conn);

//Ejecutamos la consulta SQL del archivo init.sql. Contiene la consulta para vaciar todas las tablas.
char *lecturaInitSQL();

// Funcion para vaciar/limpiar la base de datos
bool limpiarBD(PGconn *conn);

// Funcion para insertar un archivo en la base de datos
bool insertar_Archivo(PGconn *conn, char *nombre, long tamanyo, char *tipo, time_t fecha_subida, int id_usuario);

// Funcion para eliminar un archivo de la base de datos
bool eliminar_Archivo(PGconn *conn, int id_archivo);

// Funcion para convertir un timestamp a time_t
time_t strtotime(const char *timestamp_str);

// Funcion para obtener los archivos de la base de datos
Archivo* get_archivos(PGconn *conn,int *num_rows);

// Funcion para registrar un usuario en la base de datos
bool registrar_usuario1(PGconn * conn, char *nombre, char *email, char *password, time_t fecha_registro, time_t ultimo_login);

// Funcion para registrar un usuario en la base de datos
bool registrar_usuario2(PGconn * conn, char *nombre, char *email, char *password);

// Funcion para autenticar un usuario en la base de datos
bool autenticar_usuario(PGconn *conn, char *email, char *password);

Usuario* get_usuarios(PGconn *conn, int *num_rows);

#endif