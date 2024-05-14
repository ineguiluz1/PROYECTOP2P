extern "C" {
    #include "../BD/bd.h"
    #include "../GUI/menu.h"
    #include "../estructuras/Nodo/Nodo.h"
}
#include <iostream>
#include <string.h>


//gcc -c ../estructuras/Nodo/Nodo.c ../estructuras/Transferencia/Transferencia.c ../BD/bd.c ../estructuras/Archivo/Archivo.c ../estructuras/Usuario/Usuario.c ../GUI/menu.c -I "C:\Program Files\PostgreSQL\16\include" -L "C:\Program Files\PostgreSQL\16\lib" -lpq
//g++ -c admincpp.cpp -I "C:\Program Files\PostgreSQL\16\include" -L "C:\Program Files\PostgreSQL\16\lib" -lpq
//g++ -o prueba.exe *o -lstdc++ -I "C:\Program Files\PostgreSQL\16\include" -L "C:\Program Files\PostgreSQL\16\lib" -lpq


//g++ -o prueba.exe *o -I "C:\Program Files\PostgreSQL\16\include" -L "C:\Program Files\PostgreSQL\16\lib" -lpq
int main(int argc, char const *argv[])
{
    /* code */
    // Inicio de la conexion con la base de datos
    printf("Iniciando conexion con la base de datos\n");
    PGconn *conn; 
    printf("Line 22\n");
    bool estadoConexion = conexionBD(&conn);
    printf(estadoConexion ? "Conexion exitosa\n" : "Conexion fallida\n");
    printf("Line 24\n");
    if (!estadoConexion) exit(1); // Si no se pudo conectar a la BD, terminamos el programa
    Nodo nodo;
    nodo.id = 1;
    strcpy(nodo.IP_dir, "192.1.1.189");
    nodo.disponibilidad = true;
    nodo.ultima_actividad = time(NULL);
    // Inicio de la GUI
    std::cout << "Iniciando GUI" << std::endl;
    initGUI(conn);
    return 0;
}
