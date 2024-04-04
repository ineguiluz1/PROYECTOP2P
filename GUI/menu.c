#include "menu.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>
#include "../BD/bd.h"
#include <libpq-fe.h>
#include "../estructuras/Archivo/Archivo.h"
#include "../estructuras/Usuario/Usuario.h"

char nombre[20];
char passw[20];

void initGUI(PGconn *conn)
{
        /* code */
    char input[10];
    int opcion_inicio;
    int opcion_ppal;
    int sesion_iniciada = 0;

    do{
        if(!sesion_iniciada)
        {
        // Mostrar el primer menu
        mostrar_menu_login(conn);
        insertar_separador();

        // Guardar la opcion
        fgets(input, sizeof(input), stdin);
        sscanf(input, "%d", &opcion_inicio);

        // Ejecuta la opcion
        ejecutar_opcion_inicio(conn,opcion_inicio);

        if(opcion_inicio == 1 || opcion_inicio == 2)
        {
            sesion_iniciada = 1;
        }

        }else{
            
            // Mostrar el menu principal de la app
            mostrar_menu_ppal(conn);

            // Guardar la opcion en su variable correspondiente
            fgets(input, sizeof(input), stdin);
            sscanf(input, "%d", &opcion_ppal);

            // Ejecutar la opcion
            ejecutar_opcion_ppal(conn,opcion_ppal);
            
        }

    }while(opcion_inicio != 3);
}

void mostrar_menu_login(PGconn *conn)
{
    printf("MENU PRINCIPAL\n\n\n");
    printf("Bienvenido al administrador del servidor\n\n");
    insertar_separador();
    printf("1. Iniciar Sesion\n2. Registrarse\n3. Salir\n\n");
    printf("Introduce tu opcion: \n");
    
    
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

    printf("Introduce tu email: ");
    fgets(email_input, sizeof(email_input), stdin);

    // Eliminar el salto de línea del final de la entrada
    email_input[strcspn(email_input, "\n")] = '\0';

    printf("Introduce tu contrasenya: ");
    fgets(pass_input, sizeof(pass_input), stdin);

    // Eliminar el salto de línea del final de la entrada
    pass_input[strcspn(pass_input, "\n")] = '\0';

    // Verificar si el input excede los 20 caracteres
    if (strlen(pass_input) > 20) {
        printf("Error: La contraseña excede los 20 caracteres.\n");
        return;
    }

    // Copiar las credenciales introducidas en los punteros proporcionados
    strcpy(email, email_input);
    strcpy(pass, pass_input);

    //! Anyadir metodo para checkear las credenciales
    if(!autenticar_usuario(conn, email, pass))exit(0);

}

void intro_datos_registro(PGconn *conn) {
    // Datos para el registro
    char nombre[50];
    char username[50];
    char email[70];
    char passw[50];
    char passw_repeat[50];

    insertar_separador();
    // Peticion de datos y guardado en sus variables
    printf("Introduce tu nombre: ");
    fgets(nombre, sizeof(nombre), stdin);
    nombre[strcspn(nombre, "\n")] = '\0';

    printf("Introduce tu email: ");
    fgets(email, sizeof(email), stdin);
    email[strcspn(email, "\n")] = '\0';

    printf("Introduce tu nombre de usuario: ");
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = '\0';

    printf("Introduce tu contraseña: ");
    fgets(passw, sizeof(passw), stdin);
    passw[strcspn(passw, "\n")] = '\0';

    printf("Repetir contraseña: ");
    fgets(passw_repeat, sizeof(passw_repeat), stdin);
    passw_repeat[strcspn(passw_repeat, "\n")] = '\0';

    // Checkeo de las contraseñas.
    if (strcmp(passw, passw_repeat) != 0) {
        printf("Las contraseñas no coinciden. Intenta de nuevo.\n");
        return;
    }

    //! Introducir la funcion para el registro del usuario
    registrar_usuario2(conn, username, email, passw);
}

void mostrar_menu_ppal(PGconn *conn)
{
    clear_prompt();
    insertar_separador();
    printf("\nMENU PRINCIPAL\n\n");
    insertar_separador();
    printf("1. Gestion de nodos P2P\n2. Gestion de archivos\n3. Datos de las transferencias"
    "\n4. Visualizar usuarios\n5. Explorar archivos\n6. Limpiar la BD\n7. Salir\n\n");
    printf("Introduce una opcion: \n");
    insertar_separador();
}

void clear_prompt()
{
    system("cls");
}

void ejecutar_opcion_ppal( PGconn *conn,int opcion )
{
    switch(opcion)
    {
        // Caso de gestion de nodos
        case 1:
            mostrar_menu_nodos();
            break;
        // Caso de gestion de archivos
        case 2:
            mostrar_menu_archivos(conn);
            break;
        // Caso de muestra de transferencias
        case 3:
            mostrar_transferencias();
            break;
        // Caso busqueda de usuarios
        case 4:
            mostrar_explorador_usuarios();
            break;
        // Caso busqueda de archivos
        case 5:
            mostrar_explorador_archivos();
            break;
        // Limpieza de la base de datos
        case 6:
            limpiarBD(conn);
            break;
        // Salir
        case 7:
            clear_prompt();
            exit(0);
            break;
        default:
            printf("Intentalo de nuevo.\n");
            break;

    }
}

void mostrar_menu_nodos()
{
    char input[2];
    int opcion; 

    clear_prompt();
    insertar_separador();
    printf("MENU DE GESTION DE NODOS\n");
    insertar_separador();
    printf("1. Anyadir nodo\n2. Eliminar nodo\n3. Visualizar nodos\n\n");
    printf("Introduce tu opcion: \n\n");

    fgets(input, sizeof(input), stdin);
    sscanf(input,"%d", &opcion);

    switch(opcion)
    {
        case 1:

        case 2:

        case 3:

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
    printf("1. Anyadir archivos\n2. Eliminar archivos\n3. Visualizar archivos\n\n");
    printf("Introduce tu opcion: \n\n");

    fgets(input, sizeof(input), stdin);
    sscanf(input,"%d", &opcion);

    switch(opcion)
    {
        case 1:
            char input[50];
            char nombre[50];
            long tamanyo;
            char tipo[50];
            int id_usuario;
            time_t fecha_subida;

            printf("Introduce el nombre del archivo: \n\n");
            fgets(nombre, sizeof(nombre), stdin);
            nombre[strcspn(nombre, "\n")] = '\0';

            printf("Introduce el tamanyo del archivo: \n\n");
            fgets(input, sizeof(input), stdin);
            sscanf(input, "%d", tamanyo);

            printf("Introduce el tipo de archivo: ");
            fgets(tipo, sizeof(tipo), stdin);
            tipo[strcspn(tipo, "\n")] = '\0';

            printf("Introduce el tipo del archivo: \n\n");
            fgets(input, sizeof(input), stdin);
            sscanf(input, "%d", id_usuario);

            if(!insertar_Archivo(conn, nombre, tamanyo, tipo, fecha_subida, id_usuario))printf("Error al subir el archivo.\n");
            
        case 2:
            int id;
            char input_id[10];
            printf("Introduce el id del archivo que deseas eliminar: \n\n");

            fgets(input_id, sizeof(input_id), stdin);
            sscanf(input_id, "%d", id);

            if(!eliminar_Archivo(conn, id))printf("Error al imprimir archivo,\n\n");
        case 3:
            /*Archivo *archivos = get_archivos(conn,5);
            for(int i = 0; i<5; i++)
            {
                imprimirArchivo(archivos[i]);
            }*/

    }


}

void mostrar_transferencias()
{
    //! Anyadir el codigo para imprimir las transferencias salvadas de la base de datos.
}

void mostrar_explorador_usuarios()
{
    // Nombr del usuariio buscado
    char input[20];
    char usuario[20];

    mostrar_animacion("Preparando el buscador");

    insertar_separador();
    printf("\nIntroduce el usuario que deseas buscar: \n");
    fgets(input, sizeof(input), stdin);
    strcpy(usuario, input);

    printf("Estas buscando %s", usuario);

    //! Añadir la funcion para la busqueda de personas.
    
}

void mostrar_explorador_archivos()
{

    // Nombr del usuariio buscado
    char input[20];
    char archivo[20];

    insertar_separador();
    printf("\nIntroduce el archivo que deseas buscar: \n");
    fgets(input, sizeof(input), stdin);
    strcpy(archivo, input);

    printf("Estas buscando %s", archivo);

    //! Añadir la funcion para la busqueda de archivos.

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