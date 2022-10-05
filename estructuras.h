#ifndef ESTRUCTURAS_H
#define ESTRUCTURAS_H

    typedef struct jugador Jugador;
    typedef struct partida Partida;
    typedef struct lista Lista;

    struct jugador {
        int sd;
        char* nombre;
        int estado;
        /*
        estado = 0 usuario conectado no identificado (esperando USUARIO o REGISTRO)
        estado = 1 usuario registrado (esperando PASSWORD)
        estado = 2 usuario registrado (esperando INICIAR-PARTIDA) 
        estado = 3 usuario en espera de partida (esperando otro jugador en estado 3)
        estado = 4 usuario en partida
        */
        Partida* partida;
    };

    struct partida {
        char tablero[7][7];
        Jugador* jugador1;
        Jugador* jugador2;
        int turno;
    };

    struct lista{
        Jugador* item;
        Lista* sig;
    };

#endif