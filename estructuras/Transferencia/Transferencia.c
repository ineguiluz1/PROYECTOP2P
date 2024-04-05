#include "Transferencia.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


void imprimirTransferencia(Transferencia transferencia){
    printf("ID: %d\n", transferencia.id);
    printf("---------- Archivo ----------\n");
    printf("ID Archivo: %d\t Nombre archivo: %s\n", transferencia.id_archivo, transferencia.nombre_archivo);
    printf("---------- Enviado desde ----------\n");
    printf("ID Usuario Emisor: %d\t Nombre usuario Emisor: %s\n", transferencia.id_usuario_send, transferencia.nombre_usuario_send);
    printf("Id Nodo Emisor: %d\t IP Nodo Emisor: %s\n", transferencia.id_nodo_send, transferencia.ip_nodo_send);
    printf("---------- Enviado a ----------\n");
    printf("ID Usuario Receptor: %d\t Nombre usuario Receptor: %s\n", transferencia.id_usuario_rec, transferencia.nombre_usuario_rec);
    printf("Id Nodo Receptor: %d\t IP Nodo Receptor: %s\n", transferencia.id_nodo_rec, transferencia.ip_nodo_rec);
    printf("--------------------------------------------------------\n\n");
}

void imprimirTransferencia2(Transferencia transferencia){
    //printf("\tTransferencia:\n");
    printf("\tID: %d\n", transferencia.id);
    printf("\t---------- Archivo ----------\n");
    printf("\tID Archivo: %d\t Nombre archivo: %s\n", transferencia.id_archivo, transferencia.nombre_archivo);
    printf("\t---------- Enviado desde ----------\n");
    printf("\tID Usuario Emisor: %d\t Nombre usuario Emisor: %s\n", transferencia.id_usuario_send, transferencia.nombre_usuario_send);
    printf("\tId Nodo Emisor: %d\t IP Nodo Emisor: %s\n", transferencia.id_nodo_send, transferencia.ip_nodo_send);
    printf("\t---------- Enviado a ----------\n");
    printf("\tID Usuario Receptor: %d\t Nombre usuario Receptor: %s\n", transferencia.id_usuario_rec, transferencia.nombre_usuario_rec);
    printf("\tId Nodo Receptor: %d\t IP Nodo Receptor: %s\n", transferencia.id_nodo_rec, transferencia.ip_nodo_rec);
    printf("\t--------------------------------------------------------\n");
}