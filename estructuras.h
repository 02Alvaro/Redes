#ifndef ESTRUCTURAS_H
#define ESTRUCTURAS_H

    typedef struct {
        int sd;
        char* nombre;
        char* creden;
        int estatus;
        Partida* partida;
    } Jugador;

    typedef struct {
        char tablero[7][7];
        Jugador* jugador1;
        Jugador* jugador2;
        int turno;
    } Partida;

    typedef  struct{
        Jugador* item;
        Lista* sig;
    } Lista;

#endif