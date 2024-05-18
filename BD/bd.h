#include <stdio.h>
#include <stdlib.h>
#include <libpq-fe.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include "../estructuras/Archivo/Archivo.h"
#include "../estructuras/Usuario/Usuario.h"
#include "../estructuras/Nodo/Nodo.h"
#include "../estructuras/Transferencia/Transferencia.h"
#ifndef BD_H
#define BD_H

// Funcion para conectar a la base de datos
bool conexionBD(PGconn **conn);

// Funcion para vaciar/limpiar la base de datos
bool limpiarBD(PGconn *conn);

// Funcion para insertar datos de prueba en la base de datos
bool datosPruebaBD(PGconn *conn);

// Funcion para insertar un archivo en la base de datos
bool insertar_Archivo(PGconn *conn, char *nombre, long tamanyo, char *tipo, time_t fecha_subida, int id_usuario);

bool insertar_Archivo2(PGconn *conn, char *nombre, char *tamanyo, char *tipo, int id_usuario);

void eliminarFilasWhereIdUsuario(PGconn *conn,int id_usuario);

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

// Funcion para obtener los usuarios de la base de datos
Usuario* get_usuarios(PGconn *conn, int *num_rows);

// Funcion para buscar un usuario por su nombre 
Usuario* busqueda_usuarios_nombre(PGconn *conn, char *nombre, int *num_rows);

// Funcion para buscar un archivo por su nombre
Archivo* busqueda_archivos_nombre(PGconn *conn, char *nombre, int *num_rows);

// Insertar un nodo en la base de datos
bool insertar_Nodo1(PGconn *conn, char *IP_dir, time_t ultima_actividad, int id_usuario);

bool insertar_Nodo2(PGconn *conn, char *IP_dir, int id_usuario);

// Eliminar un nodo de la base de datos
bool eliminar_Nodo(PGconn *conn, int id_nodo);

// Obtener los nodos de la base de datos
Nodo* get_nodos(PGconn *conn, int *num_rows);

// Añadir usuario a nodo
bool anyadir_usuario_nodo(PGconn *conn, int id_usuario, int id_nodo);

//Insertar una transferencia en la base de datos
bool insertar_Transferencia(PGconn *conn, int id_archivo, int id_usuario_rec, int id_usuario_send, int id_nodo_rec, int id_nodo_send);

//Eliminar una transferencia de la base de datos
bool eliminar_Transferencia(PGconn *conn, int id_transferencia);

//Obtener las transferencias de la base de datos
Transferencia* get_transferencias(PGconn *conn, int *num_rows);

// Funcion para obtener los nodos en los que un usuario está registrado
Nodo *get_nodos_from_usuario(PGconn *conn, int *num_rows, int id_usuario);

// Funcion para obtener los archivos de un usuario
Archivo *get_archivos_from_usuario(PGconn *conn, int *num_rows, int id_usuario);

// Funcion para obtener las transferencias de un usuario
Transferencia *get_transferencias_from_usuario(PGconn *conn, int *num_rows, int id_usuario);

// Funcion para inicializar la base de datos
bool initBD(PGconn *conn);

// Funcion para leer script de SQL
char *lecturaScriptSQL(char* nombreArchivo);

//Funcion para obtener el ID de un usuario segun su username y password
int get_id_from_username_password(PGconn *conn,char *username,char *password);

//Mostrar nodo online
void nodo_online(PGconn *conn,char *ip, int idUsuario);

//Mostrar nodo offline
void nodo_offline(PGconn *conn,char *ip, int idUsuario);

//Eliminar nodo
void eliminar_nodo(PGconn *conn,char *ip, int idUsuario);

//
bool existeNodoDeIdUsuario_IPnodo(PGconn *conn, int *num_rows, int id_usuario, char *ip_nodo);

//Funcion para obtener los usuarios activos
Usuario *get_usuarios_activos(PGconn *conn, int* num_rows);

// Funcion para obtener los archivos disponibles
Archivo *get_archivos_disponibles(PGconn *conn, int* num_rows);

// Funcion para obtener los archivos disponibles para descarga
Archivo_descarga *get_archivos_disponibles_descarga(PGconn *conn, int *num_rows);


#endif