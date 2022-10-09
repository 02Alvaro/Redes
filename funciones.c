#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "estructuras.h"
#include "funciones.h"
#include "lista.h"

Jugador * nuevoJugador(int sd){
    Jugador* aux = (Jugador *) malloc(sizeof(Jugador));
    aux->nombre = (char*)malloc(sizeof(char)*20);
    aux->sd = sd;
    aux->estado=0;
    return aux;
}

void salirCliente(Jugador * jugador, fd_set * readfds, int * numClientes, Lista * lista){
    int socket=jugador->sd;
    char buffer[250];

    if (jugador->estado == 4){
        Jugador* contrincante;
        if(jugador->partida->jugador1->sd != socket){
            contrincante = jugador->partida->jugador1;
        } else {
            contrincante = jugador->partida->jugador2;
        }

        bzero(buffer,sizeof(buffer));
        strcpy(buffer,"-Inf. Su adversario ha salido. Use INICIAR-PARTIDA para volver a jugar\n");
        send(contrincante->sd, buffer, sizeof(buffer), 0);

        contrincante->estado = 2;
        free(contrincante->partida);
        contrincante->partida = NULL;
    }


    close(socket);
    FD_CLR(socket,readfds);
    (*numClientes)--;
    borrar(&lista, buscarJugador(lista, socket));

    bzero(buffer,sizeof(buffer));
    sprintf(buffer,"Desconexión del cliente: %d\n",socket);
    
}

int buscarUsuario(char * usuario){
    FILE * f;
    char* str1;
    char* str2;

	if((f=fopen("usuarios.txt","r"))==NULL)
        printf("Error en la opertura del fichero\n");

    while(fscanf(f,"%s,%s\n", str1, str2)>0){
        printf("leido usuario:%s|contra:%s, comparado con:%s\n",str1,str2,usuario);
        if(strcmp(str1,usuario)==0){
            fclose(f);    
            return 1;
        }
    }

    fclose(f);
    return 0;
}

int comprobarCont(char * nombre,char * psd){
    FILE * f;
    char * str1;
    char * str2;

	if((f=fopen("usuarios.txt","r"))==NULL)
        printf("Error en la opertura del fichero\n");
        
    while(fscanf(f,"%s,%s\n", str1, str2)>0){
        printf("leido usuario:%s|contra:%s, comparado con:%s\n",str1,str2,psd);
        if(strcmp(str1,nombre)==0){
            if(strcmp(str2,psd)==0){
                 fclose(f);  
                return 1;
            }else{
                 fclose(f);  
                return 0;
            }
        }
    }
    fclose(f);
    return 0;
}

void ingresarUsuario(char * nombre,char* psd ){
    FILE * f;
	if((f=fopen("usuarios.txt","r"))==NULL)
        printf("Error en la opertura del fichero\n"); 
    
    fprintf(f,"%s,%s",nombre,psd);
}
