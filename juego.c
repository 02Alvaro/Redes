#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "juego.h"
#include "estructuras.h"

void mostrarTablero(char tablero[][7],char * msg){
    char  aux;
	for(int i = 0; i < 7; i++ ){
		for(int j = 0; j < 7; j++ ){
            aux = tablero[i][j];
            if (j != 0)
                strcat(msg,",");
            strcat(msg,&aux);
		}
        strcat(msg,";");
	}
}

void inicializarPartida(Jugador* j1, Jugador* j2, char* msg){
    Partida* partida=(Partida*)malloc(sizeof(Partida));
    char tablero[7][7]={{'1','2','3','4','5','6','7'},
                        {'-','-','-','-','-','-','-'},
                        {'-','-','-','-','-','-','-'},
                        {'-','-','-','-','-','-','-'},
                        {'-','-','-','-','-','-','-'},
                        {'-','-','-','-','-','-','-'},
                        {'-','-','-','-','-','-','-'}};
    
    for(int i = 0; i < 7; i++ ){
		for(int j = 0; j < 7; j++ ){
            partida->tablero[i][j] = tablero[i][j];
		}
	}

    j1->estado = 4;
    j1->partida=partida;
    
    j2->estado = 4;
    j2->partida=partida;
    
    partida->jugador2 = j2;
    partida->jugador1 = j1;
    partida->turno = 0;

}

int actualizarTablero(char tablero[][7], int colum, int turno){
    if(colum >  6|| colum < 0)
		return -1;

    char ficha = (turno %2 == 0) ? 'x' : 'o';

	for(int i = 6; i > 0; i--){
		if(tablero[i][colum] == '-'){
            tablero[i][colum]=ficha;
			return i;
		}
	}
	return -2;
}

int finPartida(char tablero[][7],int turno,int fila,int columna){
    if(turno == MAX_TURNO){
        return 1;
    }
    char ficha = (turno %2 == 0) ? 'x' : 'o';
    int x = fila, y = columna;
    int contador = 1;

    // Comprobar vertical
    while(x < 6 && tablero[x+1][y] == ficha){
        contador++;
        x++;
    }
    if(contador >= 4)
        return 1;

	// Comprobar horizontal
    x = fila;
    contador=1;
	
    while(y > 0 && tablero[x][y-1] == ficha)
       y--;
    while(y < 6 && tablero[x][y+1] == ficha){
        contador++;
        y++;
    }
    if(contador >= 4)
        return 1;

	// Comprobar diagonal asc
    y = columna;
    contador=1;
    
    while(x > 0 && y < 6 && tablero[x-1][y + 1] == ficha){
        x--;
        y++;
    }
    while(x < 6  && y > 0 && tablero[x+1][y-1] == ficha ){
		contador++;
		x++;
		y--;
	}
    if(contador >= 4)
        return 1;

	// Comprobar diagonal desc
    x=fila;
    y=columna;
    contador=1;

    while(x > 0 && y > 0 && tablero[x-1][y - 1] == ficha){
        x--;
        y--;
    }
    while(x < 6 && y > 6 && tablero[x+1][y+1] == ficha ){
		contador++;
		x++;
		y++;
	}

    if(contador >= 4)
        return 1;
  
    return 0;
}




/*
    int  main(){
        char tablero[7][7]={{'1','2','3','4','5','6','7'},
                            {'-','-','-','-','-','-','-'},
                            {'-','-','-','-','-','-','-'},
                            {'-','-','-','-','-','-','-'},
                            {'-','-','-','-','-','-','-'},
                            {'-','-','-','-','-','-','-'},
                            {'-','-','-','-','-','-','-'}};
        int turno =0;
        int fila;
        int posicion;
        char* msg=(char*)malloc(sizeof(char)*250);
        mostrarTablero(tablero,msg);
        printf("%s",msg); 
        do{
            turno++;
            posicion=0;
            do{
                printf ("[Jugador :Introduzca la columna a tirar ficha:\n");
                scanf("%d",&posicion);
                posicion--;
                fila=actualizarTablero(tablero,posicion,turno);
            }while(fila<0);
            mostrarTablero(tablero,msg);
            printf("%s",msg); 
            printf("\n\n");
        }while(finPartida(tablero,turno,fila,posicion) == 0);
        int ganador=1;
        if(turno%2==0)
            ganador=2;
        printf("felicidades, has ganado jugador%d\n",ganador);
        return 0;

    }
*/