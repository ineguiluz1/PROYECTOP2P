#include "nodo.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>


void imprimirNodo(Nodo nodo){
    printf("ID: %d\n", nodo.id);
    printf("Direccion IP: %s\n", nodo.IP_dir);
    printf("Disponibilidad: %s\n", nodo.disponibilidad ? "Disponible" : "No disponible");
    printf("Ultima actividad: %s", asctime(localtime(&nodo.ultima_actividad)));
    printf("--------------------------------------------------------\n");
};