#ifndef LISTA_H
#define LISTA_H

    #include "estructuras.h"

    Lista *nuevoElemento();
    void insertarDetras(Lista **cabeza, Jugador* jugador);
    void insertarDelante(Lista **cabeza, Jugador * jugador);
    void borrar(Lista **cabeza, Jugador * jugador);
    int Nregistros(Lista *cabeza);
    Jugador * buscarJugador(Lista **cabeza, int sd);

#endif