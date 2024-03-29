#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>

#include "estructuras.h"
#include "juego.h"
#include "lista.h"
#include "funciones.h"

#define MSG_SIZE 250
#define MAX_CLIENTS 30
#define MAX_PARTIDAS 10

int main ( )
{
    int sd, new_sd, on;
    struct sockaddr_in sockname, from;
    socklen_t from_len;
    fd_set readfds, auxfds;

    char buffer[MSG_SIZE];
    char identificador[MSG_SIZE];
    int ret_select;

    int numClientes = 0;
    int numPartidas = 0;
    int recibidos;

    Lista *listaJugadores = NULL;

  	sd = socket (AF_INET, SOCK_STREAM, 0);
	if (sd == -1)
	{
		perror("No se puede abrir el socket cliente\n");
    		exit (1);	
	}

    on = 1;
    if (setsockopt( sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == -1){
        perror("error en la operación setsockopt");
        exit(1);
    }

	sockname.sin_family = AF_INET;
	sockname.sin_port = htons(2060);
	sockname.sin_addr.s_addr = INADDR_ANY;

	if (bind (sd, (struct sockaddr *) &sockname, sizeof (sockname)) == -1)
	{
		perror("Error en la operación bind");
		exit(1);
	}
	
	from_len = sizeof(from);


	if(listen(sd,1) == -1){
		perror("Error en la operación de listen");
		exit(1);
	}
    
    FD_ZERO(&readfds);
    FD_ZERO(&auxfds);
    FD_SET(sd,&readfds);
    FD_SET(0,&readfds);

   // signal(SIGINT,manejador);
    
    while(1){        
        auxfds = readfds;
        ret_select = select(FD_SETSIZE,&auxfds,NULL,NULL,NULL);
        
        if(ret_select > 0){
            for(int i=0; i<FD_SETSIZE; i++){
                if(FD_ISSET(i, &auxfds)) {
                    if( i == sd){ //llega un sd nuevo
                        if((new_sd = accept(sd, (struct sockaddr *)&from, &from_len)) == -1){
                            perror("Error aceptando peticiones");
                        }
                        else
                        {
                            if(numClientes < MAX_CLIENTS){
                                Jugador* jugador = nuevoJugador(new_sd);
                                insertarDetras(&listaJugadores, jugador);
                                numClientes++;
                                printf("Nuevo jugador conectado: %d/%d\n",new_sd,numClientes);
                                FD_SET(new_sd,&readfds);
                            
                                strcpy(buffer, "+Ok. Usuario conectado\n");
                                send(new_sd,buffer,sizeof(buffer),0);
                            }
                            else{ //demasiados jugadores, se rechaza conexión.
                                bzero(buffer,sizeof(buffer));
                                strcpy(buffer,"-Err. Demasiados usuarios conectados\n");
                                send(new_sd,buffer,sizeof(buffer),0);
                                close(new_sd);
                            }
                        }
                    }
                    else if (i == 0){ //escritura del servidor
                        bzero(buffer, sizeof(buffer));
                        fgets(buffer, sizeof(buffer),stdin);
                        if(strcmp(buffer,"SALIR\n") == 0){
                            for (int j = 0; j < numClientes; j++){
                                bzero(buffer, sizeof(buffer));
                                strcpy(buffer,"Desconexión servidor\n"); 
                                send(listaJugadores->item->sd, buffer, sizeof(buffer),0);
                                close(listaJugadores->item->sd);
                                FD_CLR(listaJugadores->item->sd,&readfds);
                                borrar(&listaJugadores,listaJugadores->item->sd);
                            }
                            close(sd);
                            exit(-1);
                            
                        }else{
                            for (int j = 0; j < numClientes; j++){
                                send(listaJugadores->item->sd, buffer, sizeof(buffer),0);
                            }
                        }
                    } 
                    else{ //mensaje de un cliente no nuevo
                        bzero(buffer,sizeof(buffer));
                        recibidos = recv(i,buffer,sizeof(buffer),0);

                        if(recibidos > 0 ){
                            Jugador * jugadorActual = buscarJugador(listaJugadores,i);
                            int estadoJugador = jugadorActual->estado;
                            if(buffer[strlen(buffer)-1] == '\n')
                                buffer[strlen(buffer)-1] ='\0';
                            printf("jugador[sd:%d,estado:%d]->mensaje:[%s]\n",i,estadoJugador,buffer);

                            if(strcmp(buffer,"SALIR\n") == 0){
                                //TODO
                                printf("jugador<%d> saliendo",i);
                                salirCliente(jugadorActual,&readfds,&numClientes,&listaJugadores);
                            }
                            else{
                                char* instruccion = strtok(buffer, " ");
                                if(strlen(buffer) == 0){
                                    bzero(buffer,sizeof(buffer));
                                    strcpy(buffer,"-Err. Instrucción no válida\n");
                                    send(i, buffer, sizeof(buffer), 0);
                                }
                                else if(estadoJugador == 0){
                                    if (strcmp(instruccion, "USUARIO") == 0){
                                        if((instruccion = strtok(NULL, " ")) != NULL){
                                            if(buscarUsuario(instruccion) == 1 && 
                                            buscarJugadorPorNombre(listaJugadores, instruccion) == 0){
                                                jugadorActual->estado = 1;
                                                strcpy(jugadorActual->nombre, instruccion);
                                                
                                                bzero(buffer,sizeof(buffer));
                                                strcpy(buffer,"+Ok. Usuario correcto\n");
                                                
                                            } else if (buscarJugadorPorNombre(listaJugadores, instruccion) == 1){
                                                bzero(buffer,sizeof(buffer));
                                                strcpy(buffer,"-Err. Usuario ya conectado\n");
                                            } else {
                                                bzero(buffer,sizeof(buffer));
                                                strcpy(buffer,"-Err. Usuario incorrecto\n");
                                            }
                                        } else {
                                            bzero(buffer,sizeof(buffer));
                                            strcpy(buffer,"-Err. Instrucción no válida\n");
                                        }
                                    } else if (strcmp(instruccion, "REGISTRO") == 0){
                                        int correcto = 0,error = 0, contador = 0;
                                        char usuario[250];
                                        char cont[MSG_SIZE];
                                        while(contador < 4 && error == 0){
                                            char * lineaFichero;
                                            if((instruccion = strtok(NULL, " ")) != NULL && strlen(instruccion) > 0){
                                                if(contador == 0 && strcmp(instruccion,"-u") == 0){

                                                } else if (contador == 1){
                                                    strcpy(usuario,instruccion);
                                                } else if (contador == 2 && strcmp(instruccion,"-p") == 0){

                                                } else if (contador == 3){
                                                    strcpy(cont,instruccion);
                                                } else {
                                                    error = 1;
                                                }
                                                contador++;
                                            } else {
                                                error = 1;
                                            }
                                        }

                                        //TODO
                                        if (buscarUsuario(usuario) == 1 && error == 0){
                                            bzero(buffer,sizeof(buffer));
                                            strcpy(buffer,"-Err. Usuario ya creado\n");

                                        } else if (error == 0){
                                            //TODO
                                            ingresarUsuario(usuario, cont);
                                            jugadorActual->estado = 2;
                                            bzero(buffer,sizeof(buffer));
                                            strcpy(buffer,"+Ok. Usuario registrado\n");
                                        } else {
                                            bzero(buffer,sizeof(buffer));
                                            strcpy(buffer,"-Err. Error en los argumentos\n");
                                        }
                                            
                                    } else {
                                        bzero(buffer,sizeof(buffer));
                                        strcpy(buffer,"-Err. Instrucción no válida\n");
                                    }
                                    send(i, buffer, sizeof(buffer), 0);

                                }else if(estadoJugador == 1){
                                    if (strcmp(instruccion, "PASSWORD") == 0){
                                        //TODO
                                        if((instruccion = strtok(NULL, "\0")) != NULL){
                                            if( comprobarCont(jugadorActual->nombre, instruccion) == 1 ){
                                                jugadorActual->estado = 2;
                                                bzero(buffer,sizeof(buffer));
                                                strcpy(buffer,"+Ok. Usuario validado\n");
                                            }else{
                                                bzero(buffer,sizeof(buffer));
                                                strcpy(buffer,"-Err. Error en la validación\n");
                                            }
                                        } else {
                                            bzero(buffer,sizeof(buffer));
                                            strcpy(buffer,"-Err. Instrucción no válida\n");
                                        }
                                    } else {
                                        bzero(buffer,sizeof(buffer));
                                        strcpy(buffer,"-Err. Instrucción no válida\n");
                                    }

                                    send(i, buffer, sizeof(buffer), 0);

                                }else if(estadoJugador == 2){
                                    if (strcmp(instruccion,"INICIAR-PARTIDA") == 0){
                                        Jugador* contrincante = buscarJugadorPartida(listaJugadores);
                                        if(numPartidas < MAX_PARTIDAS){
                                            if (contrincante != NULL){
                                                numPartidas++;
                                                bzero(buffer,sizeof(buffer));
                                                strcpy(buffer,"+Ok. Empieza la partida.");
                                                inicializarPartida(contrincante, jugadorActual, buffer);
                                                
                                                send(i, buffer, sizeof(buffer), 0);
                                                send(contrincante->sd, buffer, sizeof(buffer), 0);

                                                bzero(buffer,sizeof(buffer));
                                                strcpy(buffer,"+Ok. Turno de partida\n");
                                                send(contrincante->sd, buffer, sizeof(buffer), 0);
                                            } else {
                                                jugadorActual->estado = 3;
                                                bzero(buffer,sizeof(buffer));
                                                strcpy(buffer,"+Ok. Esperando jugadores\n");
                                                send(i, buffer, sizeof(buffer), 0);
                                            }
                                        } else {
                                            bzero(buffer,sizeof(buffer));
                                            strcpy(buffer,"-Err. Numero de partidas maxima\n");
                                            send(i, buffer, sizeof(buffer), 0);
                                        }
                                    } else {
                                        bzero(buffer,sizeof(buffer));
                                        strcpy(buffer,"-Err. Instrucción no válida\n");
                                        send(i, buffer, sizeof(buffer), 0);
                                    }
                                } else if (estadoJugador == 3){
                                    bzero(buffer,sizeof(buffer));
                                    strcpy(buffer,"-Err. Instrucción no válida\n");
                                    send(i, buffer, sizeof(buffer), 0);
                                
                                } else if(estadoJugador == 4){
                                    Partida* partida = jugadorActual->partida;
                                    Jugador* contrincante;

                                    contrincante = (partida->turno % 2 == 0) 
                                        ? partida->jugador1 : partida->jugador2;

                                    if (strcmp(instruccion, "COLOCAR-FICHA") == 0){
                                        if(jugadorActual->sd == contrincante->sd){
                                            bzero(buffer,sizeof(buffer));
                                            strcpy(buffer,"-Err. Debe esperar su turno\n");
                                            send(i, buffer, sizeof(buffer), 0);
                                        }else{
                                            if ((instruccion = strtok(NULL, "\0"))!= NULL){
                                                int colum=strtol(instruccion,NULL,10) - 1;
                                                int res = actualizarTablero(partida->tablero,colum,partida->turno);
                                                if(res > 0){
                                                    bzero(buffer,sizeof(buffer));
                                                    strcpy(buffer,"+Ok. Nuevo tablero.");
                                                    mostrarTablero(partida->tablero,buffer);
                                                    send(i, buffer, sizeof(buffer), 0);
                                                    send(contrincante->sd,buffer,sizeof(buffer),0);

                                                    int fin_partida = finPartida(partida->tablero,partida->turno,res,colum);
                                                   
                                                    if(fin_partida == 1){
                                                        bzero(buffer,sizeof(buffer));
                                                        sprintf(buffer,"+Ok. Jugador %s ha ganado la partida\n", jugadorActual->nombre);
                                                        send(i, buffer, sizeof(buffer), 0);
                                                        send(contrincante->sd,buffer,sizeof(buffer),0);
                                                        jugadorActual->estado = 2;
                                                        contrincante->estado = 2;
                                                        numPartidas--;

                                                    } else if(fin_partida == 2){
                                                        bzero(buffer,sizeof(buffer));
                                                        strcpy(buffer,"+Ok. Se ha producido un empate en la partida\n");
                                                        send(i, buffer, sizeof(buffer), 0);
                                                        send(contrincante->sd,buffer,sizeof(buffer),0);
                                                        jugadorActual->estado = 2;
                                                        contrincante->estado = 2;
                                                        numPartidas--;
                                                    } else {
                                                        partida->turno=partida->turno+1;
                                                        bzero(buffer,sizeof(buffer));
                                                        strcpy(buffer,"+Ok. Turno de partida.");
                                                        send(contrincante->sd,buffer,sizeof(buffer),0);
                                                    }
                                                }
                                                else if(res == -1 ){
                                                    bzero(buffer,sizeof(buffer));
                                                    strcpy(buffer,"-Err. ficha fuera de limite\n");
                                                send(i, buffer, sizeof(buffer), 0);
                                                }
                                                else if(res == -2){
                                                    bzero(buffer,sizeof(buffer));
                                                    strcpy(buffer,"-Err. Debe seleccionar otra columna que tenga alguna casilla disponible\n");
                                                    send(i, buffer, sizeof(buffer), 0);
                                                }
                                            } else {
                                                bzero(buffer,sizeof(buffer));
                                                strcpy(buffer,"-Err. Indique casilla\n");
                                                send(i, buffer, sizeof(buffer), 0);
                                            }
                                        }
                                    } else {
                                        bzero(buffer,sizeof(buffer));
                                        strcpy(buffer,"-Err. Instrucción no válida\n");
                                        send(i, buffer, sizeof(buffer), 0);
                                    }
                                }
                            }
                        }

                        if(recibidos== 0){
                            printf("El socket %d, ha introducido ctrl+c\n", i);
                            Jugador* aux =buscarJugador(listaJugadores,i);
                            if(aux->estado==4){
                                numPartidas--;
                            }
                            salirCliente(aux,&readfds,&numClientes,&listaJugadores);
                        }
                    }
                }
            }
        }
    }

	close(sd);
	return 0;
	
}