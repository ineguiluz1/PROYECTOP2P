#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <sstream>
#include <thread>
#include <vector>
#include <atomic>
#include <cstring> // Para strlen y strcmp
#include <string>
#include <sstream>
#include "../networking/servidor/Servidor.h"
#include "../networking/servidor/ServidorIndices.h"

extern "C" {
#include "../BD/bd.h"
#include "../estructuras/Archivo/Archivo.h"
}

//! Para compilar : gcc -c ../estructuras/Nodo/Nodo.c ../estructuras/Transferencia/Transferencia.c ../BD/bd.c ../estructuras/Archivo/Archivo.c ../estructuras/Usuario/Usuario.c -I "C:\Program Files\PostgreSQL\16\include" -L "C:\Program Files\PostgreSQL\16\lib" -lpq
//! g++ -c server3.cpp ../networking/servidor/*.cpp -I "C:\Program Files\PostgreSQL\16\include" -L "C:\Program Files\PostgreSQL\16\lib" -lpq -lws2_32
//! g++ -o server.exe *.o -lstdc++ -I "C:\Program Files\PostgreSQL\16\include" -L "C:\Program Files\PostgreSQL\16\lib" -lpq -lws2_32

using namespace std;



int main()
{
    ServidorIndices servidor(55555);
    servidor.iniciar();
    servidor.aceptarConexiones();
    return 0;
}
