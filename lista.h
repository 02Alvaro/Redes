#ifndef LISTA_H
#define LISTA_H

    #include "estructuras.h"

    Lista *nuevoElemento();
    void insertarDetras(Lista **cabeza, Lista* elemento);
    void insertarDelante(Lista **cabeza, Lista * elemento);
    void borrar(Lista **cabeza, Lista * elemento);
    int Nregistros(Lista *cabeza);
    Lista * buscarJugador(Lista **cabeza, int sd);

#endif