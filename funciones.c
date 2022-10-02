#include "estructuras.h"
#include "funciones.h"

Jugador * nuevoJugador(int sd){
    Jugador* aux;
    aux->sd=sd;
    aux->estado=0;
    return aux;
}