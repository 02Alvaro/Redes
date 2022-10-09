#ifndef FUNCIONES_H
#define FUNCIONES_H

    #include <sys/types.h>
    #include "estructuras.h"

    Jugador* nuevoJugador(int sd);
    int buscarUsuario(char * nombre);
    void salirCliente(Jugador * jugador, fd_set * readfds, int * numClientes, Lista * lista);
    void ingresarUsuario(char * nombre,char* psd );
    int comprobarCont(char * nombre,char * psd);

#endif