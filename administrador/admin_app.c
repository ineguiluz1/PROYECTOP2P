#include "../GUI/menu.h"
#include "../BD/bd.h"
#include <libpq-fe.h>
#include "../estructuras/Archivo/Archivo.h"
#include "../estructuras/Usuario/Usuario.h"

//!Para compilar: gcc -o admin_app.exe admin_app.c ../estructuras/Nodo/Nodo.c ../estructuras/Transferencia/Transferencia.c ../BD/bd.c ../estructuras/Archivo/Archivo.c ../estructuras/Usuario/Usuario.c ../GUI/menu.c -I "C:\Program Files\PostgreSQL\16\include" -L "C:\Program Files\PostgreSQL\16\lib" -lpq

int main(int argc, char const *argv[])
{
    /* code */

    // Inicio de la conexion con la base de datos
    PGconn *conn; 
    bool estadoConexion = conexionBD(&conn);
    if (!estadoConexion) exit(1); // Si no se pudo conectar a la BD, terminamos el programa

    // Inicio de la GUI
    initGUI(conn);
    return 0;
}
