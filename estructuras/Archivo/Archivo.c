#include "Archivo.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

Archivo crearArchivo(int id, char nombre[255], long tamanyo, char tipo[255], time_t fecha_subida, int id_usuario){
    Archivo archivo;
    archivo.id = id;
    strcpy(archivo.nombre, nombre);
    archivo.tamanyo = tamanyo;
    strcpy(archivo.tipo, tipo);
    archivo.fecha_subida = fecha_subida;
    archivo.id_usuario = id_usuario;
    return archivo;
};

void imprimirArchivo(Archivo archivo){
    printf("ID: %d\n", archivo.id);
    printf("Nombre: %s\n", archivo.nombre);
    printf("Tamanyo: %ld\n", archivo.tamanyo);
    printf("Tipo: %s\n", archivo.tipo);
    printf("Fecha de subida: %s", asctime(localtime(&archivo.fecha_subida)));
    printf("ID Usuario: %d\n", archivo.id_usuario);
    printf("--------------------------------------------------------\n");
};

void imprimirArchivo2(Archivo archivo){
    //printf("\tArchivo:\n");
    printf("\tID: %d\tNombre: %s\tTamanyo: %ld\tTipo: %s\tFecha de subida: %s\tID Usuario: %d\n", archivo.id,archivo.nombre,archivo.tamanyo,archivo.tipo,asctime(localtime(&archivo.fecha_subida)),archivo.id_usuario);
    printf("\t--------------------------------------------------------\n");
}