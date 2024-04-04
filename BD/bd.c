#include "bd.h"
#include <stdio.h>
#include <stdlib.h>
#include <libpq-fe.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include "../estructuras/Archivo/Archivo.h"
#include "../estructuras/Usuario/Usuario.h"
#include "../estructuras/Nodo/Nodo.h"


// Funcion para conectar a la base de datos
bool conexionBD(PGconn **conn) {
    const char *conninfo = "host=ep-weathered-butterfly-a2sa2f1m.eu-central-1.aws.neon.tech dbname=P2P-project-prog-IV user=P2P-project-prog-IV_owner password=3AlxMIGipb4z"; // Información de conexión
    *conn = PQconnectdb(conninfo); // Realizamos la conexión a la base de datos
    if (PQstatus(*conn) != CONNECTION_OK) { // Verificamos si la conexión fue exitosa
        fprintf(stderr, "Error de conexion: %s\n", PQerrorMessage(*conn)); // Si no fue exitosa, imprimimos el error
        PQfinish(*conn); // Finalizamos la conexión
        return false; // Retornamos false
    } else {
        printf("Conexion exitosa\n"); // Si fue exitosa, imprimimos un mensaje
        return true; // Retornamos true
    }
};


//Ejecutamos la consulta SQL del archivo init.sql. Contiene la consulta para vaciar todas las tablas.
char *lecturaInitSQL(){
    FILE *sqlFile;
    char *sqlQueries = NULL;
    size_t bufferSize = 0;
    size_t totalSize = 0;

    // Abrir el archivo SQL
    sqlFile = fopen("init.sql", "r");

    if (sqlFile == NULL) {
        printf("Error opening file!\n");
        return NULL;
    }

    // Leer y concatenar declaraciones SQL hasta el final del archivo
    char line[1024]; // Ajustar el buffer
    while (fgets(line, sizeof(line), sqlFile) != NULL) {
        // Si la línea es un comentario o está vacía, la saltamos
        if (line[0] == '\n' || line[0] == '-' || line[0] == '/' || line[0] == '\r') {
            continue;
        }

        // Eliminamos el carácter de nueva línea al final
        line[strcspn(line, "\n")] = '\0';

        // Reservamos memoria para la cadena concatenada
        totalSize += strlen(line) + 1; // +1 para el espacio entre las consultas o '\0' al final
        sqlQueries = realloc(sqlQueries, totalSize);

        // Inicializamos la memoria
        if (sqlQueries == NULL) {
            printf("Memory allocation failed!\n");
            return NULL;
        }
        if (totalSize == strlen(line) + 1) {
            strcpy(sqlQueries, line); // Si esta es la primera consulta
        } else {
            // Concatenamos la consulta actual
            strcat(sqlQueries, " ");
            strcat(sqlQueries, line);
        }
    }

    // Cerramos el archivo
    fclose(sqlFile);

    return sqlQueries;
};

// Funcion para vaciar/limpiar la base de datos
bool limpiarBD(PGconn *conn){
    printf("Entrando a limpiarBD\n");
    char *query = lecturaInitSQL(); // Leemos el archivo init.sql
    //printf("Query: %s\n", query); // Imprimimos la query
    PGresult *res = PQexec(conn, query); // Ejecutamos la query
    printf("Query ejecutada\n");
    free(query); // Liberamos la memoria de la variable query
    if (PQresultStatus(res) != PGRES_COMMAND_OK){ // Verificamos si la query fue exitosaç
        printf("Error executing query: %s\n", PQresultErrorMessage(res)); // Si no fue exitosa, imprimimos el error
        PQclear(res); // Limpiamos la variable res
        PQfinish(conn); // Finalizamos la conexión
        return false; // Retornamos false
    }
    printf("Base de datos limpiada\n"); // Si fue exitosa, imprimimos un mensaje
    PQclear(res); // Limpiamos la variable res
    return true; // Retornamos true
};

// Funcion para insertar un archivo en la base de datos
bool insertar_Archivo(PGconn *conn, char *nombre, long tamanyo, char *tipo, time_t fecha_subida, int id_usuario){
    struct tm *upload_tm = localtime(&fecha_subida);
    char formatted_timestamp[80];
    strftime(formatted_timestamp, 80, "%Y-%m-%d %H:%M:%S", upload_tm);
    char query[10000];
    sprintf(query, "INSERT INTO archivo(nombre, tamano, tipo, fecha_subida, id_usuario) VALUES('%s', %ld, '%s', '%s', %d)", nombre, tamanyo, tipo, formatted_timestamp, id_usuario);
    PGresult *res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_COMMAND_OK){
        printf("Error executing query: %s\n", PQresultErrorMessage(res));
        PQclear(res);
        PQfinish(conn);
        return false;
    }
    PQclear(res);
    return true;
};

// Funcion para eliminar un archivo de la base de datos
bool eliminar_Archivo(PGconn *conn, int id_archivo) {
    char query[1000];
    sprintf(query, "DELETE FROM archivo WHERE id = %d", id_archivo);

    PGresult *res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        //printf("Error executing query: %s\n", PQresultErrorMessage(res));
        PQclear(res);
        return false;
    }
    PQclear(res);
    return true;
};

// Funcion para convertir un timestamp a time_t
time_t strtotime(const char *timestamp_str) {
    struct tm tm;
    memset(&tm, 0, sizeof(struct tm));

    // Parsear el timestamp manualmente
    if (sscanf(timestamp_str, "%d-%d-%d %d:%d:%d", &tm.tm_year, &tm.tm_mon,
            &tm.tm_mday, &tm.tm_hour, &tm.tm_min, &tm.tm_sec) != 6) {
        // Si no se pudo parsear, retornar -1
        return (time_t)-1;
    }

    // Ajustar el año y el mes (tm_year es el número de años desde 1900, y tm_mon es 0-based)
    tm.tm_year -= 1900; // Años desde 1900
    tm.tm_mon -= 1;     // Meses desde enero (basado en 0)

    return mktime(&tm);
};

// Funcion para obtener los archivos de la base de datos
Archivo* get_archivos(PGconn *conn,int *num_rows) {
    PGresult *res = PQexec(conn, "SELECT * FROM Archivo");

    // Comprobar si la consulta fue exitosa
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        printf("Query failed: %s", PQerrorMessage(conn));
        PQclear(res);
        return NULL;
    }

    // Obtener el número de filas
    *num_rows = PQntuples(res);

    // Reservar memoria para el array de Archivos
    Archivo *archivos = malloc(*num_rows * sizeof(Archivo));
    if (archivos == NULL) {
        printf("Memory allocation failed\n");
        PQclear(res);
        return NULL;
    }

    // Llenar el array de Archivos con los datos de la consulta
    for (int i = 0; i < *num_rows; i++) {
        archivos[i].id = atoi(PQgetvalue(res, i, 0));
        strncpy(archivos[i].nombre, PQgetvalue(res, i, 1), sizeof(archivos[i].nombre) - 1);
        archivos[i].tamanyo = atol(PQgetvalue(res, i, 2));
        strncpy(archivos[i].tipo, PQgetvalue(res, i, 3), sizeof(archivos[i].tipo) - 1);
        archivos[i].fecha_subida = strtotime(PQgetvalue(res, i, 4)); // Convert string to time_t
        archivos[i].id_usuario = atoi(PQgetvalue(res, i, 5));
    }

    // Limpiar la variable res y retornar el array de Archivos
    PQclear(res);
    return archivos;
};

//Funcion para registrar un usuario en la base de datos
bool registrar_usuario1(PGconn * conn, char *nombre, char *email, char *password, time_t fecha_registro, time_t ultimo_login){
    struct tm *registro_tm = localtime(&fecha_registro);
    char formatted_registro[80];
    strftime(formatted_registro, 80, "%Y-%m-%d %H:%M:%S", registro_tm);

    struct tm *login_tm = localtime(&ultimo_login);
    char formatted_login[80];
    strftime(formatted_login, 80, "%Y-%m-%d %H:%M:%S", login_tm);

    char query[10000];
    sprintf(query, "INSERT INTO usuario(nombre, email, contrasena, fecha_registro, ultimo_login) VALUES('%s', '%s', '%s', '%s', '%s')", nombre, email, password, formatted_registro, formatted_login);
    PGresult *res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        if (strstr(PQerrorMessage(conn), "usuario_email_key")) {
            printf("Ups, that email is already picked!\n");
        } else {
            printf("Error executing query: %s\n", PQresultErrorMessage(res));
            PQfinish(conn);
        }
        PQclear(res);
        return false;
    }
    PQclear(res);
    return true;
}

//Funcion para registrar un usuario en la base de datos
bool registrar_usuario2(PGconn * conn, char *nombre, char *email, char *password){
    char query[10000];
    sprintf(query, "INSERT INTO usuario(nombre, email, contrasena) VALUES('%s', '%s', '%s')", nombre, email, password);

    PGresult *res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        if (strstr(PQerrorMessage(conn), "usuario_email_key")) {
            printf("Ups, that email is already picked!\n");
        } else {
            printf("Error executing query: %s\n", PQresultErrorMessage(res));
            PQfinish(conn);
        }
        PQclear(res);
        return false;
    }
    PQclear(res);
    return true;
}

// Funcion para autenticar un usuario
bool autenticar_usuario(PGconn *conn, char *email, char *password) {
    char query[1000];
    sprintf(query, "SELECT * FROM usuario WHERE email = '%s' AND contrasena = '%s'", email, password);

    PGresult *res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        printf("Query failed: %s", PQerrorMessage(conn));
        PQclear(res);
        return false;
    }

    int num_rows = PQntuples(res);
    PQclear(res);

    bool autenticado = num_rows > 0;
    if (autenticado) {
        
        sprintf(query, "UPDATE usuario SET ultimo_login = NOW() WHERE email = '%s'", email);
        res = PQexec(conn, query);
        if (PQresultStatus(res) != PGRES_COMMAND_OK) {
            printf("Error executing query: %s\n", PQresultErrorMessage(res));
            PQclear(res);
            return false;
        }
        PQclear(res);
    }

    return autenticado;
};

// Funcion para obtener los usuarios de la base de datos
Usuario* get_usuarios(PGconn *conn, int *num_rows) {
    PGresult *res = PQexec(conn, "SELECT * FROM usuario");

    // Comprobar si la consulta fue exitosa
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        printf("Query failed: %s", PQerrorMessage(conn));
        PQclear(res);
        return NULL;
    }

    // Obtener el número de filas
    *num_rows = PQntuples(res);

    // Reservar memoria para el array de Usuarios
    Usuario *usuarios = malloc(*num_rows * sizeof(Usuario));
    if (usuarios == NULL) {
        printf("Memory allocation failed\n");
        PQclear(res);
        return NULL;
    }

    // Llenar el array de Usuarios con los datos de la consulta
    for (int i = 0; i < *num_rows; i++) {
        usuarios[i].id = atoi(PQgetvalue(res, i, 0));
        strncpy(usuarios[i].nombre, PQgetvalue(res, i, 1), sizeof(usuarios[i].nombre) - 1);
        strncpy(usuarios[i].email, PQgetvalue(res, i, 2), sizeof(usuarios[i].email) - 1);
        strncpy(usuarios[i].contrasena, PQgetvalue(res, i, 3), sizeof(usuarios[i].contrasena) - 1);
        usuarios[i].fecha_registro = strtotime(PQgetvalue(res, i, 4)); // Convert string to time_t
        usuarios[i].ultimo_login = strtotime(PQgetvalue(res, i, 5)); // Convert string to time_t
    }

    // Limpor la variable res y retornar el array de Usuarios
    PQclear(res);
    return usuarios;
};

Usuario* busqueda_usuarios_nombre(PGconn *conn, char *nombre, int *num_rows){
    char query[1000];
    sprintf(query, "SELECT * FROM usuario WHERE nombre LIKE '%%%s%%'", nombre);
    PGresult *res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        //printf("Query failed: %s", PQerrorMessage(conn));
        PQclear(res);
        return NULL;
    }

    *num_rows = PQntuples(res);

    Usuario *usuarios = malloc(*num_rows * sizeof(Usuario));
    if (usuarios == NULL) {
        printf("Memory allocation failed\n");
        PQclear(res);
        return NULL;
    }

    for (int i = 0; i < *num_rows; i++) {
        usuarios[i].id = atoi(PQgetvalue(res, i, 0));
        strncpy(usuarios[i].nombre, PQgetvalue(res, i, 1), sizeof(usuarios[i].nombre) - 1);
        strncpy(usuarios[i].email, PQgetvalue(res, i, 2), sizeof(usuarios[i].email) - 1);
        strncpy(usuarios[i].contrasena, PQgetvalue(res, i, 3), sizeof(usuarios[i].contrasena) - 1);
        usuarios[i].fecha_registro = strtotime(PQgetvalue(res, i, 4)); // Convert string to time_t
        usuarios[i].ultimo_login = strtotime(PQgetvalue(res, i, 5)); // Convert string to time_t
    }

    PQclear(res);
    return usuarios;
};

Archivo* busqueda_archivos_nombre(PGconn *conn, char *nombre, int *num_rows){
    char query[1000];
    sprintf(query, "SELECT * FROM archivo WHERE nombre LIKE '%%%s%%'", nombre);
    PGresult *res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        //printf("Query failed: %s", PQerrorMessage(conn));
        PQclear(res);
        return NULL;
    }

    *num_rows = PQntuples(res);

    Archivo *archivos = malloc(*num_rows * sizeof(Archivo));
    if (archivos == NULL) {
        printf("Memory allocation failed\n");
        PQclear(res);
        return NULL;
    }

    for (int i = 0; i < *num_rows; i++) {
        archivos[i].id = atoi(PQgetvalue(res, i, 0));
        strncpy(archivos[i].nombre, PQgetvalue(res, i, 1), sizeof(archivos[i].nombre) - 1);
        archivos[i].tamanyo = atol(PQgetvalue(res, i, 2));
        strncpy(archivos[i].tipo, PQgetvalue(res, i, 3), sizeof(archivos[i].tipo) - 1);
        archivos[i].fecha_subida = strtotime(PQgetvalue(res, i, 4)); // Convert string to time_t
        archivos[i].id_usuario = atoi(PQgetvalue(res, i, 5));
    }

    PQclear(res);
    return archivos;
};

// Insertar un nodo en la base de datos
bool insertar_Nodo1(PGconn *conn, char *IP_dir, time_t ultima_actividad, int id_usuario){
    struct tm *actividad_tm = localtime(&ultima_actividad);
    char formatted_actividad[80];
    strftime(formatted_actividad, 80, "%Y-%m-%d %H:%M:%S", actividad_tm);
    char query[10000];
    sprintf(query, "INSERT INTO nodo(IP_DIR, DISPONIBILIDAD, ULTIMA_ACTIVIDAD) VALUES('%s', FALSE, '%s') RETURNING id", IP_dir, formatted_actividad);
    PGresult *res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_COMMAND_OK){
        //printf("Error executing query: %s\n", PQresultErrorMessage(res));
        PQclear(res);
        PQfinish(conn);
        return false;
    }
    int id_generado = atoi(PQgetvalue(res, 0, 0));
    char query2[10000];
    sprintf(query2, "INSERT INTO usuarionodo(id_usuario,id_nodo) VALUES(%d, %d)", id_usuario, id_generado);
    res = PQexec(conn, query2);
    if (PQresultStatus(res) != PGRES_COMMAND_OK){
        //printf("Error executing query: %s\n", PQresultErrorMessage(res));
        PQclear(res);
        PQfinish(conn);
        return false;
    }
    PQclear(res);
    return true;
};

// Insertar un nodo en la base de datos
bool insertar_Nodo2(PGconn *conn, char *IP_dir, int id_usuario){
    char query[10000];
    sprintf(query, "INSERT INTO nodo(IP_DIR, DISPONIBILIDAD, ULTIMA_ACTIVIDAD) VALUES('%s', FALSE, NOW()) RETURNING id", IP_dir);
    PGresult *res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_TUPLES_OK){
        // printf("Error executing query 1: %s\n", PQresultErrorMessage(res));
        PQclear(res);
        PQfinish(conn);
        return false;
    }
    int id_generado = atoi(PQgetvalue(res, 0, 0));
    PQclear(res);
    char query2[10000];
    sprintf(query2, "INSERT INTO usuarionodo(ultima_conexion,id_usuario,id_nodo) VALUES(NOW(),%d, %d)", id_usuario, id_generado);
    res = PQexec(conn, query2);
    if (PQresultStatus(res) != PGRES_COMMAND_OK){
        // printf("Error executing query 2: %s\n", PQresultErrorMessage(res));
        PQclear(res);
        PQfinish(conn);
        return false;
    }
    PQclear(res);
    return true;
};

// Eliminar un nodo de la base de datos
bool eliminar_Nodo(PGconn *conn, int id_nodo) {
    char query[1000];
    sprintf(query, "DELETE FROM nodo WHERE id = %d", id_nodo);

    PGresult *res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        //printf("Error executing query: %s\n", PQresultErrorMessage(res));
        PQclear(res);
        return false;
    }
    PQclear(res);
    return true;
};


// Obtener los nodos de la base de datos
Nodo *get_nodos(PGconn *conn, int *num_rows)
{
    PGresult *res = PQexec(conn, "SELECT * FROM nodo");

    // Comprobar si la consulta fue exitosa
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        printf("Query failed: %s", PQerrorMessage(conn));
        PQclear(res);
        return NULL;
    }

    // Obtener el número de filas
    *num_rows = PQntuples(res);

    // Reservar memoria para el array de Nodos
    Nodo *nodos = malloc(*num_rows * sizeof(Nodo));
    if (nodos == NULL) {
        printf("Memory allocation failed\n");
        PQclear(res);
        return NULL;
    }

    // Llenar el array de Nodos con los datos de la consulta
    for (int i = 0; i < *num_rows; i++) {
        nodos[i].id = atoi(PQgetvalue(res, i, 0));
        strncpy(nodos[i].IP_dir, PQgetvalue(res, i, 1), sizeof(nodos[i].IP_dir) - 1);
        nodos[i].disponibilidad = PQgetvalue(res, i, 2)[0] == 't';
        nodos[i].ultima_actividad = strtotime(PQgetvalue(res, i, 3)); // Convert string to time_t
    }

    // Limpiar la variable res y retornar el array de Nodos
    PQclear(res);
    return nodos;
};

// Añadir usuario a nodo
bool anyadir_usuario_nodo(PGconn *conn, int id_usuario, int id_nodo){
    char query[1000];
    sprintf(query, "INSERT INTO usuarionodo(ultima_conexion,id_usuario, id_nodo) VALUES(NOW(),%d, %d)", id_usuario, id_nodo);
    PGresult *res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        //printf("Error executing query: %s\n", PQresultErrorMessage(res));
        PQclear(res);
        PQfinish(conn);
        return false;
    }
    PQclear(res);
    return true;
};

//Insertar una transferencia en la base de datos
bool insertar_Transferencia(PGconn *conn, int id_archivo, int id_usuario_rec, int id_usuario_send, int id_nodo_rec, int id_nodo_send)
{
    char query[10000];
    sprintf(query, "INSERT INTO transferencia(id_archivo, id_usuario_rec, id_usuario_send, id_nodo_rec, id_nodo_send) VALUES(%d, %d, %d, %d, %d)", id_archivo, id_usuario_rec, id_usuario_send, id_nodo_rec, id_nodo_send);
    PGresult *res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        //printf("Error executing query: %s\n", PQresultErrorMessage(res));
        PQclear(res);
        PQfinish(conn);
        return false;
    }
    PQclear(res);
    return true;
};

//Eliminar una transferencia de la base de datos
bool eliminar_Transferencia(PGconn *conn, int id_transferencia){
    char query[1000];
    sprintf(query, "DELETE FROM transferencia WHERE id = %d", id_transferencia);
    PGresult *res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        //printf("Error executing query: %s\n", PQresultErrorMessage(res));
        PQclear(res);
        return false;
    }
    PQclear(res);
    return true;
};

// Funcion para obtener el nombre de un usuario por su id
char* get_nombre_usuario(PGconn *conn, int id_usuario){
    char query[1000];
    sprintf(query, "SELECT nombre FROM usuario WHERE id = %d", id_usuario);
    PGresult *res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        //printf("Query failed: %s", PQerrorMessage(conn));
        PQclear(res);
        return NULL;
    }
    char* nombre = malloc(strlen(PQgetvalue(res, 0, 0)) + 1); // Asignar memoria suficiente
    if (nombre == NULL) {
        // Manejo de error si malloc falla
        PQclear(res);
        return NULL;
    }
    strcpy(nombre, PQgetvalue(res, 0, 0)); 
    PQclear(res); 
    return nombre; 
};

// Funcion para obtener la IP de un nodo por su id
char* get_ip_nodo(PGconn *conn, int id_nodo){
    char query[1000];
    sprintf(query, "SELECT ip_dir FROM nodo WHERE id = %d", id_nodo);
    PGresult *res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        //printf("Query failed: %s", PQerrorMessage(conn));
        PQclear(res);
        return NULL;
    }
    char* ip = malloc(strlen(PQgetvalue(res, 0, 0)) + 1); // Asignar memoria suficiente
    if (ip == NULL) {
        // Manejo de error si malloc falla
        PQclear(res);
        return NULL;
    }
    strcpy(ip, PQgetvalue(res, 0, 0)); 
    PQclear(res); 
    return ip; 
};

// Funcion para obtener el nombre de un archivo por su id
char* get_nombre_archivo(PGconn *conn, int id_archivo){
    char query[1000];
    sprintf(query, "SELECT nombre FROM archivo WHERE id = %d", id_archivo);
    PGresult *res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        //printf("Query failed: %s", PQerrorMessage(conn));
        PQclear(res);
        return NULL;
    }
    char* nombre = malloc(strlen(PQgetvalue(res, 0, 0)) + 1); // Asignar memoria suficiente
    if (nombre == NULL) {
        // Manejo de error si malloc falla
        PQclear(res);
        return NULL;
    }
    strcpy(nombre, PQgetvalue(res, 0, 0)); 
    PQclear(res); 
    return nombre; 
};

Transferencia* get_transferencias(PGconn *conn, int *num_rows){
    PGresult *res = PQexec(conn, "SELECT * FROM transferencia");

    // Comprobar si la consulta fue exitosa
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        printf("Query failed: %s", PQerrorMessage(conn));
        PQclear(res);
        return NULL;
    }

    // Obtener el número de filas
    *num_rows = PQntuples(res);

    // Reservar memoria para el array de Transferencias
    Transferencia *transferencias = malloc(*num_rows * sizeof(Transferencia));
    if (transferencias == NULL) {
        printf("Memory allocation failed\n");
        PQclear(res);
        return NULL;
    }

    // Llenar el array de Transferencias con los datos de la consulta
    for (int i = 0; i < *num_rows; i++) {
        transferencias[i].id = atoi(PQgetvalue(res, i, 0));
        transferencias[i].id_archivo = atoi(PQgetvalue(res, i, 1));
        transferencias[i].nombre_archivo = get_nombre_archivo(conn, transferencias[i].id_archivo);
        transferencias[i].id_usuario_rec = atoi(PQgetvalue(res, i, 2));
        transferencias[i].nombre_usuario_rec = get_nombre_usuario(conn, transferencias[i].id_usuario_rec);
        transferencias[i].id_usuario_send = atoi(PQgetvalue(res, i, 3));
        transferencias[i].nombre_usuario_send = get_nombre_usuario(conn, transferencias[i].id_usuario_send);
        transferencias[i].id_nodo_rec = atoi(PQgetvalue(res, i, 4));
        transferencias[i].ip_nodo_rec = get_ip_nodo(conn, transferencias[i].id_nodo_rec);
        transferencias[i].id_nodo_send = atoi(PQgetvalue(res, i, 5));
        transferencias[i].ip_nodo_send = get_ip_nodo(conn, transferencias[i].id_nodo_send);
    }

    // Limpiar la variable res y retornar el array de Transferencias
    PQclear(res);
    return transferencias;
};


// Nodo* get_nodo_from_id_Nodo(PGconn *conn, int id_nodo) {
//     char query[1000];
//     sprintf(query, "SELECT * FROM nodo WHERE id=%d", id_nodo);
//     PGresult *res = PQexec(conn, query);
//     if (PQresultStatus(res) != PGRES_TUPLES_OK) {
//         //printf("Query failed: %s", PQerrorMessage(conn));
//         PQclear(res);
//         // Devolver un nodo "vacío" o con valores predeterminados si la consulta falla
//         return NULL;
//     }

//     Nodo *nodo;
//     nodo->id = atoi(PQgetvalue(res, 0, 0));
//     strncpy(nodo->IP_dir, PQgetvalue(res, 0, 1), sizeof(nodo->IP_dir) - 1);
//     nodo->disponibilidad = PQgetvalue(res, 0, 2)[0] == 't';
//     nodo->ultima_actividad = strtotime(PQgetvalue(res, 0, 3)); // Convertir cadena a time_t
//     PQclear(res);
//     imprimirNodo(*nodo);
//     return nodo;
// }



// // Funcion para obtener los nodos en los que un usuario está registrado
// Nodo *get_nodos_from_usuario(PGconn *conn, int *num_rows, int id_usuario){
//     char query[1000];
//     sprintf(query, "SELECT id_nodo FROM usuarionodo WHERE id_usuario=%d", id_usuario);
//     PGresult *res = PQexec(conn, query);
//     if (PQresultStatus(res) != PGRES_TUPLES_OK) {
//         printf("Query failed: %s", PQerrorMessage(conn));
//         PQclear(res);
//         return NULL;
//     }

//     *num_rows = PQntuples(res);
//     printf("num_rows: %d\n", *num_rows);
//     Nodo *nodos = malloc(*num_rows * sizeof(Nodo));
//     if (nodos == NULL) {
//         printf("Memory allocation failed\n");
//         PQclear(res);
//         return NULL;
//     }

//     for (int i = 0; i < *num_rows; i++) {
//         printf("Aquiedf");
//         nodos[i] = *get_nodo_from_id_Nodo(conn, atoi(PQgetvalue(res, i, 0)));
//         printf("Aqui");
//     }
// };

Nodo* get_nodo_from_id_Nodo(PGconn *conn, int id_nodo) {
    char query[1000];
    sprintf(query, "SELECT * FROM nodo WHERE id=%d", id_nodo);
    PGresult *res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        printf("Query failed: %s", PQerrorMessage(conn));
        PQclear(res);
        return NULL;
    }

    Nodo *nodo = malloc(sizeof(Nodo));
    if (nodo == NULL) {
        printf("Memory allocation failed\n");
        PQclear(res);
        return NULL;
    }

    nodo->id = atoi(PQgetvalue(res, 0, 0));
    strncpy(nodo->IP_dir, PQgetvalue(res, 0, 1), sizeof(nodo->IP_dir) - 1);
    nodo->disponibilidad = PQgetvalue(res, 0, 2)[0] == 't';
    nodo->ultima_actividad = strtotime(PQgetvalue(res, 0, 3)); // Convertir cadena a time_t
    PQclear(res);
    imprimirNodo(*nodo);
    return nodo;
};

Nodo *get_nodos_from_usuario(PGconn *conn, int *num_rows, int id_usuario){
    char query[1000];
    sprintf(query, "SELECT id_nodo FROM usuarionodo WHERE id_usuario=%d", id_usuario);
    PGresult *res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        printf("Query failed: %s", PQerrorMessage(conn));
        PQclear(res);
        return NULL;
    }

    *num_rows = PQntuples(res);
    printf("num_rows: %d\n", *num_rows);
    Nodo *nodos = malloc(*num_rows * sizeof(Nodo));
    if (nodos == NULL) {
        printf("Memory allocation failed\n");
        PQclear(res);
        return NULL;
    }

    for (int i = 0; i < *num_rows; i++) {
        nodos[i] = *get_nodo_from_id_Nodo(conn, atoi(PQgetvalue(res, i, 0)));
    }
    PQclear(res);
    return nodos;
};