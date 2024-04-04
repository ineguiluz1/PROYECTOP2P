#include <time.h>
#ifndef ARCHIVO_H
#define ARCHIVO_H

typedef struct {
    int id;
    char nombre[255];
    long tamanyo;
    char tipo[255];
    time_t fecha_subida;
    int id_usuario;
} Archivo;

Archivo crearArchivo(int id, char nombre[255], long tamanyo, char tipo[255], time_t fecha_subida, int id_usuario);

void imprimirArchivo(Archivo archivo);

#endif