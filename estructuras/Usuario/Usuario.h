#ifndef USUARIO_H
#define USUARIO_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct {
    int id;
    char nombre[255]; 
    char email[255]; 
    char contrasena[255]; 
    time_t fecha_registro;
    time_t ultimo_login;  
} Usuario;

// Funcion para imprimir un usuario
void imprimirUsuario(Usuario usuario);

#endif