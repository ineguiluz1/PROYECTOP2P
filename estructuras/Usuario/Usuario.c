#include "Usuario.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


void imprimirUsuario(Usuario usuario){
    printf("ID: %d\n", usuario.id);
    printf("Nombre: %s\n", usuario.nombre);
    printf("Email: %s\n", usuario.email);
    printf("Contrasena: %s\n", usuario.contrasena);
    printf("Fecha de registro: %s", asctime(localtime(&usuario.fecha_registro)));
    printf("Ultimo login: %s\n", asctime(localtime(&usuario.ultimo_login)));
    printf("--------------------------------------------------------\n");
};