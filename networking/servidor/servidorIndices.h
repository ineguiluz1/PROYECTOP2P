#ifndef SERVIDORINDICES_H
#define SERVIDORINDICES_H

#include "Servidor.h"

class ServidorIndices : public Servidor {
public:
    // Constructor
    ServidorIndices(int puerto);

    // Destructor
    ~ServidorIndices();

    // Método sobrescrito para manejar la conexión con un cliente
    void manejarCliente(SOCKET socketCliente,char *clientIP);
};

#endif // SERVIDOR INDICES_H
