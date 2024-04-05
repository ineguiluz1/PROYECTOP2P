#include <time.h>
#ifndef TRANSFERENCIA_H
#define TRANSFERENCIA_H

typedef struct {
    int id;
    int id_archivo; // id del archivo que se transfiere
    char *nombre_archivo; // nombre del archivo que se transfiere
    int id_usuario_rec; // id del usuario que recibe la transferencia
    char *nombre_usuario_rec; // nombre del usuario que recibe la transferencia
    int id_usuario_send; // id del usuario que envia la transferencia
    char *nombre_usuario_send; // nombre del usuario que envia la transferencia
    int id_nodo_rec; // id del nodo que recibe la transferencia
    char *ip_nodo_rec; // ip del nodo que recibe la transferencia
    int id_nodo_send; // id del nodo que envia la transferencia
    char *ip_nodo_send; // ip del nodo que envia la transferencia
} Transferencia;


void imprimirTransferencia(Transferencia transferencia);
void imprimirTransferencia2(Transferencia transferencia);

#endif