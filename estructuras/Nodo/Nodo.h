#ifndef NODO_H
#define NODO_H
#include <stdbool.h>
#include <time.h>

typedef struct
{
    int id;
    char IP_dir[255];
    bool disponibilidad;
    time_t ultima_actividad;

}Nodo;

void imprimirNodo(Nodo nodo);
#endif