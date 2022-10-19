#include"lista.h"
#include "estructuras.h"
#include <stdio.h>
#include <stdlib.h>   
#include <string.h>

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

void borrar(Lista **cabeza, int sd){
    Lista *aux=*cabeza;
    if((*cabeza)->item->sd == sd){
        *cabeza = (*cabeza)->sig;
        return;
    }
    while(aux->sig->item->sd != sd){
        aux=aux->sig;
    }
    aux->sig = aux->sig->sig;
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
int buscarJugadorPorNombre(Lista *cabeza, char * nombre){
    Lista * aux=cabeza;
    while(aux != NULL){
        if(strcmp(aux->item->nombre,nombre) == 0){
            return 1;
        }
        aux=aux->sig;
    }
    return 0;
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
void printLista(Lista * cabeza){
    Lista * aux = cabeza;
    while(aux != NULL){
        printf("Jugador[sd:%d,estado:%d,nombre:%s,partida:%p]\n",
            aux->item->sd,aux->item->estado,aux->item->nombre,aux->item->partida);
        aux=aux->sig;
    }
}

Jugador * nuevoJugaador(int sd,char *nombre,int estado){
    Jugador* jugador = (Jugador*)malloc(sizeof(jugador));
    jugador->sd = sd;
    jugador->nombre = nombre;
    jugador->estado = estado;
    //jugador->partida =NULL;
    return jugador;
}


/*
int main(){
    Lista * lista = NULL;
    char msg[250];
    Jugador* j1 = nuevoJugaador(1,"alvaro",0);
    Jugador* j2 = nuevoJugaador(2,"mario",1);
    Jugador* j3 = nuevoJugaador(3,"manu",2);
    Jugador* j4 = nuevoJugaador(4,"jose",3);
    insertarDetras(&lista,j1);
    insertarDetras(&lista,j2);
    insertarDetras(&lista,j3);
    insertarDetras(&lista,j4);
    borrar(&lista,4);
    printLista(lista);
}
*/