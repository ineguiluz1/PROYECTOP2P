#include "../GUI/menu.h"
#include "../BD/bd.h"
#include <libpq-fe.h>
#include "../estructuras/Archivo/Archivo.h"
#include "../estructuras/Usuario/Usuario.h"

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
