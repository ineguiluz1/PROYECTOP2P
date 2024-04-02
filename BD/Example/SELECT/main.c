//Ejemplo conexion BD remota. Necesaria la instalación de PostgreSQL
#include <stdio.h>
#include <stdlib.h>
#include <libpq-fe.h> // Incluimos la biblioteca libpq
//! Para compilar: gcc -o main.exe main.c -I "C:\Program Files\PostgreSQL\16\include" -L "C:\Program Files\PostgreSQL\16\lib" -lpq
int main() {
    printf("Conexión a base de datos remota\n");
    // Declaramos las variables necesarias
    PGconn *conn;
    const char *conninfo = "host=ep-weathered-butterfly-a2sa2f1m.eu-central-1.aws.neon.tech dbname=P2P-project-prog-IV user=P2P-project-prog-IV_owner password=3AlxMIGipb4z"; // Información de conexión

    // Realizamos la conexión a la base de datos
    conn = PQconnectdb(conninfo);
    // Verificamos si la conexión fue exitosa
    if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "Error de conexión: %s\n", PQerrorMessage(conn));
        PQfinish(conn);
        exit(1);
    }else{
        printf("Conexión exitosa\n");
    }

    char *query = "SELECT * FROM playing_with_neon";

    // Submit the query and retrieve the result
    PGresult *res = PQexec(conn, query);

    // Check the status of the query result
    ExecStatusType resStatus = PQresultStatus(res);

    // Convert the status to a string and print it
    printf("Query Status: %s\n", PQresStatus(resStatus));

    // Check if the query execution was successful
    if (resStatus != PGRES_TUPLES_OK) {
        // If not successful, print the error message and finish the connection
        printf("Error while executing the query: %s\n", PQerrorMessage(conn));

        // Clear the result
        PQclear(res);

        // Finish the connection
        PQfinish(conn);

        // Exit the program
        exit(1);
    }

    // We have successfully executed the query
    printf("Query Executed Successfully\n");

    // Get the number of rows and columns in the query result
    int rows = PQntuples(res);
    int cols = PQnfields(res);
    printf("Number of rows: %d\n", rows);
    printf("Number of columns: %d\n", cols);

    // Print the column names
    for (int i = 0; i < cols; i++) {
        printf("%s\t", PQfname(res, i));
    }
    printf("\n");

    // Print all the rows and columns
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            // Print the column value
            printf("%s\t", PQgetvalue(res, i, j));
        }
        printf("\n");
    }

    // Clear the result
    PQclear(res);

    // Finish the connection
    PQfinish(conn);

    return 0;


}
