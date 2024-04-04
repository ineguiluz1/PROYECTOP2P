#include <stdio.h>
#include <stdlib.h>
#include <libpq-fe.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include "../estructuras/Archivo/Archivo.h"
#include "../estructuras/Usuario/Usuario.h"

//! Para compilar: gcc -o main.exe main.c ../estructuras/Archivo/Archivo.c ../estructuras/Usuario/Usuario.c -I "C:\Program Files\PostgreSQL\16\include" -L "C:\Program Files\PostgreSQL\16\lib" -lpq
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
}


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
}

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
}

// Funcion para eliminar un archivo de la base de datos
bool eliminar_Archivo(PGconn *conn, int id_archivo) {
    char query[1000];
    sprintf(query, "DELETE FROM archivo WHERE id_archivo = %d", id_archivo);

    PGresult *res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        printf("Error executing query: %s\n", PQresultErrorMessage(res));
        PQclear(res);
        return false;
    }
    PQclear(res);
    return true;
}

// Funcion para convertir un timestamp a time_t
// Function to convert timestamp string to time_t
time_t strtotime(const char *timestamp_str) {
    struct tm tm;
    memset(&tm, 0, sizeof(struct tm));

    // Parse the timestamp string manually
    if (sscanf(timestamp_str, "%d-%d-%d %d:%d:%d", &tm.tm_year, &tm.tm_mon,
            &tm.tm_mday, &tm.tm_hour, &tm.tm_min, &tm.tm_sec) != 6) {
        // Parsing failed
        return (time_t)-1;
    }

    // Adjust year and month (tm_year is the number of years since 1900, and tm_mon is 0-based)
    tm.tm_year -= 1900; // Years since 1900
    tm.tm_mon -= 1;     // Months (0-based index)

    return mktime(&tm);
}

Archivo* obtenerArchivos(PGconn *conn,int *num_rows) {
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

    // Limpor la variable res y retornar el array de Archivos
    PQclear(res);
    return archivos;
}

// bool registrar_usuario(PGconn *conn, char *username, char *password) {
//     char query[1000];
//     sprintf(query, "INSERT INTO usuario(username, password) VALUES('%s', '%s')", username, password);

//     PGresult *res = PQexec(conn, query);
//     if (PQresultStatus(res) != PGRES_COMMAND_OK) {
//         printf("Error executing query: %s\n", PQresultErrorMessage(res));
//         PQclear(res);
//         return false;
//     }
//     PQclear(res);
//     return true;
// }
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
}

int main(int argc, char const *argv[]) {
    PGconn *conn;
    bool estadoConexion = conexionBD(&conn);
    if (!estadoConexion) exit(1); // Si no se pudo conectar a la BD, terminamos el programa

    //printf("Limpiando tablas:\n");

    // bool estadoLimpieza = limpiarBD(conn);
    // printf("Limpieza de la base de datos: %s\n", estadoLimpieza ? "Exitosa" : "Fallida"); // Imprimimos el estado de la limpieza de la BD

    // struct tm date = {0}; // Initialize struct tm

    // // Set the date components
    // date.tm_year = 2020 - 1900; // Years since 1900
    // date.tm_mon = 0; // January (0-based index)
    // date.tm_mday = 15; // Day of the month

    // // Set the time components
    // date.tm_hour = 15; // Hour (24-hour format)
    // date.tm_min = 9; // Minute
    // date.tm_sec = 45; // Second

    // // Call mktime to convert struct tm to time_t
    // time_t time = mktime(&date);

    // bool estadoArchivo = insertar_Archivo(conn, "Archivo12ex.txt", 1024, "txt", time, 1);
    // printf("Subida de Archivo: %s\n", estadoArchivo ? "Exitosa" : "Fallida");

    // int num_rows = 0;
    // Archivo *archivos = obtenerArchivos(conn,&num_rows);
    
    // for (int i = 0; i < num_rows; i++) {
    //     imprimirArchivo(archivos[i]);
    // }

    // free(archivos);

    autenticar_usuario(conn, "aaa","bbb");

    int num_rows = 0;
    Usuario *usuarios = get_usuarios(conn, &num_rows);
    for (int i = 0; i < num_rows; i++) {
        imprimirUsuario(usuarios[i]);
    }
    free(usuarios);



    if (autenticar_usuario(conn, "aaa","bbb")) {
        printf("Usuario autenticado\n");
    } else {
        printf("Usuario no autenticado\n");
    }
    registrar_usuario2(conn, "juan.perez@correo.com", "aaa","bbb");



    PQfinish(conn); // Finalizamos la conexión
    printf("Conexion finalizada\n"); // Imprimimos un mensaje
    return 0;
}

