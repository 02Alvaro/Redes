#ifndef JUEGO_H
#define JUEGO_H

    #include "estructuras.h"

    #define MAX_TURNO 42

    void mostrarTablero(char tablero[][7],char * msg);
    void inicializarPartida(Jugador* j1, Jugador* j2, char* msg);
    int actualizarTablero(char tablero[][7], int colum, int turno);
    int finPartida(char tablero[][7],int turno,int fila,int columna);

#endif