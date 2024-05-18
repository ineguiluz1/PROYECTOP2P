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

typedef struct {
    int id;
    char nombre[255];
    long tamanyo;
    char tipo[255];
    time_t fecha_subida;
    int id_usuario;
    char ip_dir[20];
} Archivo_descarga;


Archivo crearArchivo(int id, char nombre[255], long tamanyo, char tipo[255], time_t fecha_subida, int id_usuario);

void imprimirArchivo(Archivo archivo);
void imprimirArchivo2(Archivo archivo);

#endif