#include"lista.h"
#include "estructuras.h"
#include <stdlib.h>   

Lista *nuevoElemento(){
    return ( Lista*)malloc(sizeof(Lista));
}

void insertarDetras(Lista **cabeza, Lista* elemento){
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

void insertarDelante(Lista **cabeza, Lista * elemento){
    if(*cabeza == NULL){
        *cabeza = elemento;
        return;
    }
    elemento->sig = *cabeza;
    *cabeza = elemento;
}

void borrar(Lista **cabeza, Lista * elemento){
    Lista *aux=*cabeza;
    if(*cabeza == elemento){
        *cabeza = elemento->sig;
        return;
    }
    while(aux->sig != elemento){
        aux=aux->sig;
    }
    aux->sig = elemento->sig;
    free(elemento);
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

Jugador * buscarJugador(Lista **cabeza, int sd){
    Lista * aux;
    while(aux->sig != NULL){
        if(aux->item->sd == sd){
            return aux->item;
        }
        aux=aux->sig;
    }
    return NULL;
}