#ifndef JUEGO_H
#define JUEGO_H

    #define MAX_TURNO 42

    void mostrarTablero(char tablero[][7],char * msg);
    int actualizarTablero(char tablero[][7], int colum, int turno);
    int finPartida(char tablero[][7],int turno,int fila,int columna);

#endif