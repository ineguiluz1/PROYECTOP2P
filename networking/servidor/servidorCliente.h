#ifndef SERVIDORCLIENTE_H
#define SERVIDORCLIENTE_H

#include "Servidor.h"

class ServidorCliente : public Servidor {
public:
    // Constructor
    ServidorCliente(int puerto);

    // Destructor
    ~ServidorCliente();

    // Método sobrescrito para manejar la conexión con un cliente
    void manejarCliente(SOCKET socketCliente,char *clientIP);
};

#endif // SERVIDOR INDICES_H
