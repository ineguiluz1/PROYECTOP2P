#ifndef SERVIDOR_H
#define SERVIDOR_H

#include <string>
#include <winsock2.h>
#include <atomic>
#include <libpq-fe.h>

class Servidor {
public:
    // Constructor
    Servidor(int puerto);

    // Destructor
    ~Servidor();

    // Método para iniciar el servidor
    bool iniciar();

    // Método para aceptar conexiones
    void aceptarConexiones();

private:
    int puerto;
    int socketServidor;
    struct sockaddr_in direccionServidor;
    std::atomic<int> clientNumber{0};
    PGconn *conn;

    // Método para cerrar el servidor
    void cerrarServidor();

    // Método para manejar la conexión con un cliente
    virtual void manejarCliente(SOCKET socketCliente, char *clientIP) = 0;
};

#endif // SERVIDOR_H
