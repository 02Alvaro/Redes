#include"lista.h"
#include "estructuras.h"
#include <stdio.h>
#include <stdlib.h>   

Lista *nuevoElemento(){
    return ( Lista*)malloc(sizeof(Lista));
}

void insertarDetras(Lista **cabeza, Jugador *jugador){
    Lista *elemento = nuevoElemento();
    elemento -> item = jugador;
    Lista *aux  =  *cabeza;
    if( aux == NULL){
        *cabeza = elemento;
        return;
    }
    while(aux->sig != NULL){
        aux = aux->sig;
    }
    aux->sig = elemento;
}

void insertarDelante(Lista **cabeza, Jugador *jugador){
    Lista *elemento = nuevoElemento();
    elemento -> item = jugador;
    elemento->sig = *cabeza;
    *cabeza = elemento;
}

void borrar(Lista **cabeza, Jugador *jugador){
    Lista *aux=*cabeza;
    if((*cabeza)->item == jugador){
        *cabeza = (*cabeza)->sig;
        free(jugador);
        return;
    }
    while(aux->sig->item != jugador){
        aux=aux->sig;
    }

    //TODO comprobar que funciona
    Lista *elementoABorrar=aux->sig;
    aux->sig = aux->sig->sig;
    free(jugador);
    free(elementoABorrar);
}

int Nregistros(Lista *cabeza){
    int contador = 0;
    Lista *aux = cabeza;
    while(aux != NULL){
        aux = aux->sig;
        contador++;
    }
    return contador;
} 

Jugador * buscarJugador(Lista *cabeza, int sd){
    Lista * aux=cabeza;
    while(aux != NULL){
        if(aux->item->sd == sd){
            return aux->item;
        }
        aux=aux->sig;
    }
    return NULL;
}
Jugador * buscarJugadorPartida(Lista  * cabeza){
    Lista * aux=cabeza;
    while(aux != NULL){
        if(aux->item->estado == 3){
            return aux->item;
        }
        aux=aux->sig;
    }
    return NULL;
}