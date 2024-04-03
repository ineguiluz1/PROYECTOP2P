#include <stdio.h>
#include <stdlib.h>
#include <libpq-fe.h>
#include <string.h>
#include <stdbool.h>
//! Para compilar: gcc -o main.exe main.c -I "C:\Program Files\PostgreSQL\16\include" -L "C:\Program Files\PostgreSQL\16\lib" -lpq
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
    printf("Query: %s\n", query); // Imprimimos la query
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


int main(int argc, char const *argv[]) {
    PGconn *conn;
    bool estadoConexion = conexionBD(&conn);
    if (!estadoConexion) exit(1); // Si no se pudo conectar a la BD, terminamos el programa

    printf("Limpiando tablas:\n");

    bool estadoLimpieza = limpiarBD(conn);
    printf("Limpieza de la base de datos: %s\n", estadoLimpieza ? "Exitosa" : "Fallida"); // Imprimimos el estado de la limpieza de la BD

    PQfinish(conn); // Finalizamos la conexión

    return 0;
}

