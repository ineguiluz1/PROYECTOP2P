#include "menu.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>
#include "../BD/bd.h"
#include <libpq-fe.h>
#include "../estructuras/Archivo/Archivo.h"
#include "../estructuras/Usuario/Usuario.h"
#include "../estructuras/Nodo/Nodo.h"
#include "../estructuras/Transferencia/Transferencia.h"


char nombre[20];
char passw[20];

void clearIfNeeded(char *str, int max_line)
{
	// Limpia los caracteres de más introducidos
	if ((strlen(str) == max_line-1) && (str[max_line-2] != '\n'))
		while (getchar() != '\n');
}


void initGUI(PGconn *conn){
    //         /* code */
    char input[10];
    int opcion_inicio;
    int opcion_ppal;
    bool sesion_iniciada = 0;
    do{
        clear_prompt();
        mostrar_menu_login(conn);
        insertar_separador();

        // Guardar la opcion
        fgets(input, sizeof(input), stdin);
        sscanf(input, "%d", &opcion_inicio);

        // Ejecuta la opcion
        ejecutar_opcion_inicio(conn,opcion_inicio);
    }while(opcion_inicio != 3);
}

void mostrar_menu_login(PGconn *conn)
{
    printf("MENU PRINCIPAL\n\n\n");
    printf("Bienvenido al administrador del servidor\n\n");
    insertar_separador();
    printf("1. Iniciar Sesion\n2. Registrarse\n3. Salir\n\n");
    printf("Introduce tu opcion: \n");
    fflush(stdout);
}

void insertar_separador()
{
    printf("=========================================\n");
}

void ejecutar_opcion_inicio( PGconn *conn, int opcion )
{
    switch(opcion)
    {
        // Caso login
        case 1:
            intro_credenciales(conn,nombre, passw);
            break;
        // Caso registro
        case 2:
            intro_datos_registro(conn);
            break;
        // Salir
        case 3:
            clear_prompt();
            exit(0);
            break;
        // Caso en el que no se registre ni 1 ni 2 ni 3
        default:
            printf("Opcion invalida\n");
            break;
    }
}

void intro_credenciales(PGconn*conn, char *email, char *pass)
{
    char email_input[60];
    char pass_input[20];
    printf("\nINICIO DE SESION\n");
    insertar_separador();   
    printf("Introduce tu email: ");
    fflush(stdout);
    fgets(email_input, sizeof(email_input), stdin);
    clearIfNeeded(email_input, sizeof(email_input));

    // Eliminar el salto de línea del final de la entrada
    email_input[strcspn(email_input, "\n")] = '\0';

    printf("Introduce tu contrasenya: ");
    fflush(stdout);
    fgets(pass_input, sizeof(pass_input), stdin);
    clearIfNeeded(pass_input, sizeof(pass_input));

    // Eliminar el salto de línea del final de la entrada
    pass_input[strcspn(pass_input, "\n")] = '\0';

    // Verificar si el input excede los 20 caracteres
    if (strlen(pass_input) > 20) {
        printf("Error: La contrasenya excede los 20 caracteres.\n");
        printf("Pulsa Enter para reintentar...\n");
        fflush(stdout);
        while(getchar() != '\n');
        return;
    }

    // Copiar las credenciales introducidas en los punteros proporcionados
    strcpy(email, email_input);
    strcpy(pass, pass_input);

    if(autenticar_usuario(conn, email, pass)){
        mostrar_menu_ppal(conn);
    }else{
        printf("Usuario o contrasenya incorrecta\n");
        printf("Pulsa Enter para reintentar...\n");
        fflush(stdout);
        while(getchar() != '\n');
    }

}

void intro_datos_registro(PGconn *conn) {
    // Datos para el registro
    char nombre[50];
    char email[70];
    char passw[50];
    char passw_repeat[50];

    printf("\nREGISTRO DE USUARIO\n");
    insertar_separador();
    // Peticion de datos y guardado en sus variables
    printf("Introduce tu nombre: ");
    fgets(nombre, sizeof(nombre), stdin);
    clearIfNeeded(nombre, sizeof(nombre));
    nombre[strcspn(nombre, "\n")] = '\0';

    printf("Introduce tu email: ");
    fgets(email, sizeof(email), stdin);
    clearIfNeeded(email, sizeof(email));
    email[strcspn(email, "\n")] = '\0';

    printf("Introduce tu contrasenya: ");
    fgets(passw, sizeof(passw), stdin);
    clearIfNeeded(passw, sizeof(passw));
    passw[strcspn(passw, "\n")] = '\0';

    printf("Repetir contrasenya: ");
    fgets(passw_repeat, sizeof(passw_repeat), stdin);
    clearIfNeeded(passw_repeat, sizeof(passw_repeat));
    passw_repeat[strcspn(passw_repeat, "\n")] = '\0';

    // Checkeo de las contraseñas.
    if (strcmp(passw, passw_repeat) != 0) {
        printf("Las contrasenyas no coinciden. Intenta de nuevo.\n");
        return;
    }

    if(registrar_usuario2(conn, nombre, email, passw)){
        mostrar_menu_ppal(conn);
    }else{
        printf("Error al registrar el usuario\n");
        printf("Pulsa Enter para reintentar...\n");
        fflush(stdout);
        while(getchar() != '\n');
    }
}

void mostrar_menu_ppal(PGconn *conn)
{   do{
    char input[2];
    int opcion_ppal;
    clear_prompt();
    insertar_separador();
    printf("\nMENU PRINCIPAL\n\n");
    insertar_separador();
    printf("1. Gestion de nodos P2P\n2. Gestion de archivos\n3. Gestion de transferencias"
    "\n4. Explorar usuarios\n5. Limpiar la BD\n6. Visualizacion completa\n7. Insertar datos de prueba\n8. Salir\n\n");
    printf("Introduce una opcion: \n");
    insertar_separador();
    fflush(stdout);
    fgets(input, sizeof(input), stdin);
    sscanf(input, "%d", &opcion_ppal);
    clearIfNeeded(input, sizeof(input));
    // Ejecutar la opcion
    ejecutar_opcion_ppal(conn,opcion_ppal);
    }while(1);
}

void clear_prompt()
{
    system("cls");
}



void mostrar_menu_nodos(PGconn *conn)
{
    char input[2];
    int opcion; 

    clear_prompt();
    insertar_separador();
    printf("MENU DE GESTION DE NODOS\n");
    insertar_separador();
    printf("1. Crear nodo\n2. Eliminar nodo\n3. Visualizar nodos\n4. Anyadir nodo a usuario\n5. Volver\n\n");
    printf("Introduce tu opcion: \n\n");
    fflush(stdout);
    fgets(input, sizeof(input), stdin);
    sscanf(input,"%d", &opcion);
    clearIfNeeded(input, sizeof(input));
    manageMenuNodes(opcion, conn);
}

void manageMenuNodes(int opcion, PGconn *conn)
{
    switch (opcion)
    {
    case 1:
    {
        char input[255];
        char dir_ip[255];
        char input_id[10];
        int id_usuario;
        printf("Introduce la direccion IP del nodo: \n\n");
        fflush(stdout);
        fgets(input, sizeof(input), stdin);
        clearIfNeeded(input, sizeof(input));
        input[strcspn(input, "\n")] = '\0';
        strcpy(dir_ip, input);

        printf("Introce el id del usuario: \n\n");
        fflush(stdout);
        fgets(input_id, sizeof(input_id), stdin);
        sscanf(input_id, "%i", &id_usuario);
        clearIfNeeded(input_id, sizeof(input_id));

        if (!insertar_Nodo2(conn, dir_ip, id_usuario))
        {
            printf("Error al insertar el nodo.\n");
            fflush(stdout);
            fgets(input_id, sizeof(input_id), stdin);
            sscanf(input_id, "%i", &id_usuario);
            clearIfNeeded(input_id, sizeof(input_id));
            if(!eliminar_Nodo(conn, id_usuario)){
                printf("Error al eliminar nodo\n");
                fflush(stdout);
            }else{
                printf("Nodo con id = %i eliminado con exito\n",id_usuario);
                fflush(stdout);
            }
            printf("Pulsa Enter para salir...\n");
            fflush(stdout);
            while(getchar() != '\n');
            break;
        }
        else
        {
            printf("Nodo anyadido con exito\n");
            fflush(stdout);
        }
        printf("Pulsa Enter para salir...\n");
        fflush(stdout);
        while (getchar() != '\n')
            ;
        break;
    }
    case 2:
    {
        int id;
        char input_id[10];
        printf("Introduce el id del nodo que deseas eliminar: \n\n");
        fflush(stdout);
        fgets(input_id, sizeof(input_id), stdin);
        sscanf(input_id, "%i", &id);
        clearIfNeeded(input_id, sizeof(input_id));
        if (!eliminar_Nodo(conn, id))
        {
            printf("Error al eliminar nodo\n");
            fflush(stdout);
        }
        else
        {
            printf("Nodo con id = %i eliminado con exito\n");
            fflush(stdout);
        }
        printf("Pulsa Enter para salir...\n");
        fflush(stdout);
        while (getchar() != '\n')
            ;
        break;
    }
    case 3:
    {
        int nrows = 0;
        Nodo *nodos = get_nodos(conn, &nrows);
        for (int i = 0; i < nrows; i++)
        {
            imprimirNodo(nodos[i]);
        }
        free(nodos);
        printf("\nPulsa Enter para salir...\n");
        fflush(stdout);
        while (getchar() != '\n')
            ;
        break;
    }
    case 4:
    {
        char input_nodo[10];
        int id_nodo;
        char input_id[10];
        int id_usuario;

        printf("Introduce ID del nodo: \n\n");
        fflush(stdout);
        fgets(input_nodo, sizeof(input_nodo), stdin);
        sscanf(input_nodo, "%i", &id_nodo);
        clearIfNeeded(input_nodo, sizeof(input_nodo));

        printf("Introce el ID del usuario: \n\n");
        fflush(stdout);
        fgets(input_id, sizeof(input_id), stdin);
        sscanf(input_id, "%i", &id_usuario);
        clearIfNeeded(input_id, sizeof(input_id));

        if (!anyadir_usuario_nodo(conn, id_usuario, id_nodo))
        {
            printf("Error al anyadir el usuario al nodo.\n");
            fflush(stdout);
        }
        else
        {
            printf("Usuario anyadido al nodo con exito. \n");
            fflush(stdout);
        }

        printf("\nPulsa Enter para salir...\n");
        fflush(stdout);
        while (getchar() != '\n')
            ;
        break;
    }
    case 5:
        break;
    default:
        break;
    }
}

void mostrar_menu_archivos(PGconn *conn)
{
    char input[2];
    int opcion; 

    clear_prompt();
    insertar_separador();
    printf("MENU DE GESTION DE ARCHIVOS\n");
    insertar_separador();
    printf("1. Anyadir archivos\n2. Eliminar archivos\n3. Visualizar todos los archivos\n4. Busqueda de archivos por nombre\n5. Volver\n\n");
    printf("Introduce tu opcion: \n\n");
    fflush(stdout);
    fgets(input, sizeof(input), stdin);
    sscanf(input,"%d", &opcion);
    clearIfNeeded(input, sizeof(input));
    switch(opcion)
    {
        case 1:
        {
            char input[50];
            char nombre[50];
            long tamanyo;
            char tipo[50];
            int id_usuario;
            time_t fecha_subida;

            printf("Introduce el nombre del archivo: \n\n");
            fflush(stdout);
            fgets(nombre, sizeof(nombre), stdin);
            clearIfNeeded(nombre, sizeof(nombre));
            nombre[strcspn(nombre, "\n")] = '\0';

            printf("Introduce el tamanyo del archivo: \n\n");
            fflush(stdout);
            fgets(input, sizeof(input), stdin);
            sscanf(input, "%ld", &tamanyo);
            clearIfNeeded(input, sizeof(input));

            printf("Introduce el tipo de archivo: \n\n");
            fflush(stdout);
            fgets(tipo, sizeof(tipo), stdin);
            clearIfNeeded(tipo, sizeof(tipo));
            tipo[strcspn(tipo, "\n")] = '\0';

            printf("Introduce el id del usuario propietario: \n\n");
            fflush(stdout);
            fgets(input, sizeof(input), stdin);
            sscanf(input, "%i", &id_usuario);
            clearIfNeeded(input, sizeof(input));

            if(!insertar_Archivo(conn, nombre, tamanyo, tipo, fecha_subida, id_usuario)){
                printf("Error al subir el archivo.\n");
                fflush(stdout);
            }
            break;
        }
        case 2:
        {
            int id;
            char input_id[10];
            printf("Introduce el id del archivo que deseas eliminar: \n\n");
            fflush(stdout);
            fgets(input_id, sizeof(input_id), stdin);
            sscanf(input_id, "%i", &id);
            clearIfNeeded(input_id, sizeof(input_id));
            if(!eliminar_Archivo(conn, id)){
                printf("Error al eliminar archivo");
                fflush(stdout);
            }else{
                printf("Archivo con id = %i eliminado con exito\n");
                fflush(stdout);
            }
            printf("Pulsa Enter para salir...\n");
            fflush(stdout);
            while(getchar() != '\n');
            break;
        }
        case 3:
        {   
            int nrows = 0;
            Archivo *archivos = get_archivos(conn,&nrows);
            for(int i = 0; i<nrows; i++)
            {
                imprimirArchivo(archivos[i]);
            }
            free(archivos);
            printf("\nPulsa Enter para salir...\n");
            fflush(stdout);
            while(getchar() != '\n');
            break;
        }
            
        case 4:
        {
            char input_archivo[20];
            char archivo[20];

            insertar_separador();
            fflush(stdout);
            printf("\nIntroduce el archivo que deseas buscar: \n");
            fflush(stdout);
            clearIfNeeded(input_archivo, sizeof(input_archivo));
            fgets(input_archivo, sizeof(input_archivo), stdin);
            input_archivo[strcspn(input_archivo, "\n")] = '\0';
            clearIfNeeded(input_archivo, sizeof(input_archivo));
            strcpy(archivo, input_archivo);
            printf("Busqueda de %s:\n", archivo);
            mostrar_animacion("Buscando");
            fflush(stdout);
            int num_rows = 0;
            Archivo *archivos = busqueda_archivos_nombre(conn, archivo, &num_rows);
            if (archivos == NULL || num_rows == 0) {
                printf("No se encontraron archivos con ese nombre.\n");
                fflush(stdout);
            } else {
                for (int i = 0; i < num_rows; ++i) {
                    imprimirArchivo(archivos[i]);
                }
                fflush(stdout);
                free(archivos);
            }
            

            printf("Pulsa Enter para salir...\n");
            fflush(stdout);
            while(getchar() != '\n');
            break;
        }
        case 5:
            break;
        default:
            break;
    }


}

void mostrar_transferencias(PGconn *conn)
{
    char input[2];
    int opcion; 

    clear_prompt();
    insertar_separador();
    printf("MENU DE GESTION DE TRANSFERENCIAS\n");
    insertar_separador();
    printf("1. Anyadir transferencia\n2. Eliminar transferencia\n3. Visualizar todas las transferencias\n4. Volver\n\n");
    printf("Introduce tu opcion: \n\n");
    fflush(stdout);
    fgets(input, sizeof(input), stdin);
    sscanf(input,"%d", &opcion);
    clearIfNeeded(input, sizeof(input));
    switch(opcion)
    {
        case 1:
        {   
            char input[10];
            int id_archivo;
            int id_usuario_rec;
            int id_nodo_rec;
            int id_usuario_send;
            int id_nodo_send;

            printf("Introduce el id del archivo: \n\n");
            fflush(stdout);
            fgets(input, sizeof(input), stdin);
            sscanf(input, "%i", &id_archivo);
            clearIfNeeded(input, sizeof(input));

            printf("Introduce el id del usuario receptor: \n\n");
            fflush(stdout);
            fgets(input, sizeof(input), stdin);
            sscanf(input, "%i", &id_usuario_rec);
            clearIfNeeded(input, sizeof(input));

            printf("Introduce el id del nodo receptor: \n\n");
            fflush(stdout);
            fgets(input, sizeof(input), stdin);
            sscanf(input, "%i", &id_nodo_rec);
            clearIfNeeded(input, sizeof(input));

            printf("Introduce el id del usuario emisor: \n\n");
            fflush(stdout);
            fgets(input, sizeof(input), stdin);
            sscanf(input, "%i", &id_usuario_send);
            clearIfNeeded(input, sizeof(input));

            printf("Introduce el id del nodo emisor: \n\n");
            fflush(stdout);
            fgets(input, sizeof(input), stdin);
            sscanf(input, "%i", &id_nodo_send);
            clearIfNeeded(input, sizeof(input));

            if(!insertar_Transferencia(conn, id_archivo, id_usuario_rec, id_usuario_send, id_nodo_rec, id_nodo_send)){
                printf("Error al insertar la transferencia.\n");
                fflush(stdout);
            }else{
                printf("Transferencia anyadida con exito\n");
                fflush(stdout);
            }

            printf("Pulsa Enter para salir...\n");
            fflush(stdout);
            while(getchar() != '\n');
            break;
        }
        case 2:
        {   
            char input[10];
            int id_transferencia;

            printf("Introduce el id de la transferencia que deseas eliminar: \n\n");
            fflush(stdout);
            fgets(input, sizeof(input), stdin);
            sscanf(input, "%i", &id_transferencia);
            clearIfNeeded(input, sizeof(input));

            if(!eliminar_Transferencia(conn, id_transferencia)){
                printf("Error al eliminar la transferencia.\n");
                fflush(stdout);
            }else{
                printf("Transferencia con id = %i eliminada con exito\n",id_transferencia);
                fflush(stdout);
            }

            printf("Pulsa Enter para salir...\n");
            fflush(stdout);
            while(getchar() != '\n');
            break;
        }
        case 3:
        {
            int nrows = 0;
            Transferencia *transferencias = get_transferencias(conn,&nrows);
            for(int i = 0; i<nrows; i++)
            {
                imprimirTransferencia(transferencias[i]);
            }

            for(int i = 0; i<nrows; i++){
                free(transferencias[i].nombre_archivo);
                free(transferencias[i].nombre_usuario_rec);
                free(transferencias[i].nombre_usuario_send);
                free(transferencias[i].ip_nodo_rec);
                free(transferencias[i].ip_nodo_send);
            }
            free(transferencias);

            printf("Pulsa Enter para salir...\n");
            fflush(stdout);
            while(getchar() != '\n');
            break;
        }
        case 4:
            break;
        default:
            break;
    }
}

void mostrar_explorador_usuarios(PGconn *conn)
{
    // Nombr del usuariio buscado
    char input[2];
    int opcion; 

    clear_prompt();
    insertar_separador();
    clear_prompt();
    insertar_separador();
    printf("MENU DE EXPLORACION DE USUARIOS\n");
    insertar_separador();
    printf("1. Visualizar todos los usuarios\n2. Buscar usuarios por nombre\n3. Volver\n\n");
    printf("Introduce tu opcion: \n\n");
    fflush(stdout);
    fgets(input, sizeof(input), stdin);
    sscanf(input,"%d", &opcion);
    clearIfNeeded(input, sizeof(input));
    switch(opcion)
    {
        case 1:
        {
            int nrows = 0;
            Usuario *usuarios = get_usuarios(conn,&nrows);
            for(int i = 0; i<nrows; i++)
            {
                imprimirUsuario(usuarios[i]);
            }
            free(usuarios);
            printf("\nPulsa Enter para salir...\n");
            fflush(stdout);
            while(getchar() != '\n');
            break;
        }
        case 2:
        {
            char input_usuario[20];
            char usuario[20];

            insertar_separador();
            fflush(stdout);
            printf("\nIntroduce el usuario que deseas buscar: \n");
            fflush(stdout);
            clearIfNeeded(input_usuario, sizeof(input_usuario));
            fgets(input_usuario, sizeof(input_usuario), stdin);
            input_usuario[strcspn(input_usuario, "\n")] = '\0';
            clearIfNeeded(input_usuario, sizeof(input_usuario));
            strcpy(usuario, input_usuario);
            printf("Busqueda de %s:\n", usuario);
            mostrar_animacion("Buscando");
            fflush(stdout);
            //! Añadir la funcion para la busqueda de personas.
            int num_rows = 0;
            Usuario *usuarios = busqueda_usuarios_nombre(conn, usuario, &num_rows);
            if (usuarios == NULL || num_rows == 0) {
                printf("No se encontraron usuarios con ese nombre.\n");
                fflush(stdout);
            } else {
                for (int i = 0; i < num_rows; ++i) {
                    imprimirUsuario(usuarios[i]);
                }
                fflush(stdout);
                free(usuarios);
            }
            

            printf("Pulsa Enter para salir...\n");
            fflush(stdout);
            while(getchar() != '\n');
            break;
        }
        case 3:
            break;
        default:
            break;
    }
}

// Definición de la función mostrar_animacion
void mostrar_animacion( char* Texto ) {
    printf("%s", Texto);
    fflush(stdout); // Limpiar el buffer de salida para que el texto se imprima inmediatamente
    
    // Animación: puntos que aparecen y desaparecen
    for (int i = 0; i < 2; ++i) {
        Sleep(500); // Esperar medio segundo
        printf(".");
        Sleep(500);
        printf(".");
        Sleep(500);
        printf(".");
        fflush(stdout);
        Sleep(500); // Esperar medio segundo
        printf("\b \b"); // Retroceder el cursor para borrar el punto
        Sleep(500);
        printf("\b \b");
        Sleep(500);
        printf("\b \b");
        fflush(stdout);
    }
    printf("\n"); // Agregar una nueva línea al final de la animación
}

void visualizacion_completa(PGconn *conn)
{
    printf("VISUALIZACION COMPLETA POR USUARIOS\n");
    printf("=====================================\n");
    int nrows = 0;
    Usuario *usuarios = get_usuarios(conn,&nrows);
    for(int i = 0; i<nrows; i++)
    {   
        imprimirUsuario2(usuarios[i]);
        int nrow2 = 0;
        Nodo *nodos = get_nodos_from_usuario(conn,&nrow2,usuarios[i].id);
        int j = 0;
        printf("\tNodos:\n");
        for(j = 0; j<nrow2; j++)
        {
            imprimirNodo2(nodos[j]);
        }
        free(nodos);
        int nrow3 = 0;
        Archivo *archivos = get_archivos_from_usuario(conn,&nrow3,usuarios[i].id);
        if (nrow3 > 0) {
            printf("\tArchivos:\n");
        }
        for(j = 0; j<nrow3; j++)
        {
            imprimirArchivo2(archivos[j]);
        }
        free(archivos);
        
        int nrows4 = 0;
        Transferencia *transferencias = get_transferencias(conn,&nrows4);
        if (nrows4 > 0) {
            printf("\tTransferencias:\n");
        }
        for(j = 0; j<nrows4; j++)
        {
            imprimirTransferencia2(transferencias[j]);
        }  
        free(transferencias);

    }
    printf("Pulsa Enter para salir...\n");
    fflush(stdout);
    while(getchar() != '\n');
}

void confirmar_limpiarBD(PGconn *conn){
    char input[2];
    char opcion;
    clear_prompt();
    printf("LIMPIAR BASE DE DATOS\n");
    printf("=====================================\n");
    printf("Estas seguro de que deseas limpiar la base de datos, esta operacion eliminaras todos los datos de la Base de Datos sin poder recuperarlos, ¿deseas continuar?\n[s/n]\n");
    fflush(stdout);
    fgets(input, sizeof(input), stdin);
    sscanf(input,"%c", &opcion);
    clearIfNeeded(input, sizeof(input));
    if(opcion == 's'){
        printf("Eliminando datos de la base de datos...\n");
        if (limpiarBD(conn)){
            printf("Base de datos elminada\n");
        }else{
            printf("Error al eliminar la base de datos\n");
        }
        
    }else{
        printf("Operacion cancelada\n");
    }
    printf("Pulsa Enter para salir...\n");
    fflush(stdout);
    while(getchar() != '\n');
}

void insertar_datos_prueba(PGconn *conn)
{   
    if(datosPruebaBD(conn)){
        printf("Datos de prueba insertados con exito\n");
    }else{
        printf("Error al insertar los datos de prueba\n");
    }
    printf("Pulsa Enter para salir...\n");
    fflush(stdout);
    while(getchar() != '\n');
}

void ejecutar_opcion_ppal( PGconn *conn,int opcion )
{
    switch(opcion)
    {
        // Caso de gestion de nodos
        case 1:
            mostrar_menu_nodos(conn);
            break;
        // Caso de gestion de archivos
        case 2:
            mostrar_menu_archivos(conn);
            break;
        // Caso de muestra de transferencias
        case 3:
            mostrar_transferencias(conn);
            break;
        // Caso busqueda de usuarios
        case 4:
            mostrar_explorador_usuarios(conn);
            break;
        // Limpieza de la base de datos
        case 5:
            confirmar_limpiarBD(conn);
            limpiarBD(conn);
            break;
        // Visualizar toda la informacion de la base de datos, por usuarios
        case 6:
            visualizacion_completa(conn);
            break;
        // Insertar datos de prueba
        case 7:
            insertar_datos_prueba(conn);
            break;
        // Salir
        case 8:
            clear_prompt();
            exit(0);
            break;
        default:
            printf("Intentalo de nuevo.\n");
            break;
    }
}