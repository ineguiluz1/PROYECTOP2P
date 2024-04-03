//Ejemplo conexion BD remota. Necesaria la instalación de PostgreSQL
#include <stdio.h>
#include <stdlib.h>
#include <libpq-fe.h> // Incluimos la biblioteca libpq
//! Para compilar: gcc -o main.exe main.c -I "C:\Program Files\PostgreSQL\16\include" -L "C:\Program Files\PostgreSQL\16\lib" -lpq
//! Para ejecutar el .exe: añadir a Path en Variables de Entorno del Sistema: C:\Program Files\PostgreSQL\16\bin
int main() {
    printf("Conexion a base de datos remota\n");
    // Declaramos las variables necesarias
    PGconn *conn;
    PGresult *res;
    const char *conninfo = "host=ep-weathered-butterfly-a2sa2f1m.eu-central-1.aws.neon.tech dbname=P2P-project-prog-IV user=P2P-project-prog-IV_owner password=3AlxMIGipb4z"; // Información de conexión
    // Realizamos la conexión a la base de datoss
    conn = PQconnectdb(conninfo);
    // Verificamos si la conexión fue exitosa
    if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "Error de conexion: %s\n", PQerrorMessage(conn));
        PQfinish(conn);
        exit(1);
    }else{
        printf("Conexion exitosa\n");
    }
    
    res = PQexec(conn, "INSERT INTO playing_with_neon(name,value) VALUES('PRUEBA-SUBIDA',0.36747)");

    if (PQresultStatus(res) != PGRES_COMMAND_OK){
        fprintf(stderr, "Error executing query: %s\n", PQresultErrorMessage(res));
        PQclear(res);
        PQfinish(conn);
        exit(1);
    }

        
    PQclear(res);
}