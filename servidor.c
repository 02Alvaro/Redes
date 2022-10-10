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
#define MAX_CLIENTS 50

void manejador(int signum);


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
    
    //int arrayClientes[MAX_CLIENTS];
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
                            
                        }
                    } 
                    else{ //mensaje de un cliente no nuevo
                        bzero(buffer,sizeof(buffer));
                        recibidos = recv(i,buffer,sizeof(buffer),0);

                        if(recibidos > 0){
                            Jugador * jugadorActual = buscarJugador(listaJugadores,i);
                            int estadoJugador = jugadorActual->estado;
                            printf("jugador[sd:%d,estado:%d]->mensaje:[%s]\n",i,estadoJugador,buffer);

                            if(strcmp(buffer,"SALIR\n") == 0){
                                //TODO
                                printf("jugador<%d> saliendo",i);
                                salirCliente(jugadorActual,&readfds,&numClientes,&listaJugadores);
                            }
                            else{
                                char* instruccion = strtok(buffer, " ");
                                printf("Instruccion leida:%s\n",instruccion);
                                if(estadoJugador == 0){
                                    if (strcmp(instruccion, "USUARIO") == 0){
                                        instruccion= strtok(NULL, " ");
                                        if(instruccion[strlen(instruccion)-1] == '\n')
                                            instruccion[strlen(instruccion)-1] ='\0';
                                        if(buscarUsuario(instruccion) == 1){
                                            jugadorActual->estado = 1;
                                            printf("leido:%s\n",instruccion);
                                            strcpy(jugadorActual->nombre, instruccion);
                                            
                                            bzero(buffer,sizeof(buffer));
                                            strcpy(buffer,"+Ok. Usuario correcto\n");
                                            
                                        } else {
                                            bzero(buffer,sizeof(buffer));
                                            strcpy(buffer,"-Err. Usuario incorrecto\n");
                                        }
                                        printf("entrando al USUARIO\n");
                                        
                                    } else if (strcmp(instruccion, "REGISTRO") == 0){
                                        int correcto = 0,error = 0, contador = 0;
                                        char usuario[250];
                                        char cont[MSG_SIZE];
                                        while(correcto == 0 && error == 0){
                                            char * lineaFichero;
                                            instruccion = strtok(NULL, " ");
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
                                        }

                                        //TODO
                                        if (buscarUsuario(usuario) == 1){
                                            bzero(buffer,sizeof(buffer));
                                            strcpy(buffer,"-Err. Usuario ya creado\n");

                                        } else {
                                            //TODO
                                            ingresarUsuario(usuario, cont);
                                            jugadorActual->estado = 2;
                                            bzero(buffer,sizeof(buffer));
                                            strcpy(buffer,"+Ok. Usuario registrado\n");
                                        }                    
                                            
                                    } else {
                                        bzero(buffer,sizeof(buffer));
                                        strcpy(buffer,"-Err. Instrucción no válida\n");
                                    }
                                    printf("<%d>:%s:",i,buffer);
                                    send(i, buffer, sizeof(buffer), 0);

                                }else if(estadoJugador == 1){
                                    if (strcmp(instruccion, "PASSWORD") == 0){
                                        //TODO
                                        instruccion =  strtok(NULL, "\n");
                                        if(instruccion[strlen(instruccion)-1] == '\n')
                                            instruccion[strlen(instruccion)-1] ='\0';
                                        if( comprobarCont(jugadorActual->nombre,instruccion) == 1 ){
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

                                    send(i, buffer, sizeof(buffer), 0);

                                }else if(estadoJugador == 2){
                                    if(instruccion[strlen(instruccion)-1] == '\n')
                                            instruccion[strlen(instruccion)-1] ='\0';
                                    if (strcmp(instruccion,"INICIAR-PARTIDA") == 0){
                                        //TODO
                                        // Busca en orden de aparición en la lista el
                                        // primer jugador con estado 3
                                        Jugador* contrincante = buscarJugadorPartida(listaJugadores);
                                        if (contrincante != NULL){
                                            bzero(buffer,sizeof(buffer));
                                            inicializarPartida(jugadorActual, contrincante, buffer);
                                            
                                            send(i, buffer, sizeof(buffer), 0);
                                            send(contrincante->sd, buffer, sizeof(buffer), 0);

                                            bzero(buffer,sizeof(buffer));
                                            strcpy(buffer,"+Ok. Turno de partida\n");
                                            send(i, buffer, sizeof(buffer), 0);
                                        } else {
                                            jugadorActual->estado = 3;
                                        }
                                    } else {
                                        bzero(buffer,sizeof(buffer));
                                        strcpy(buffer,"-Err. Instrucción no válida\n");
                                        send(i, buffer, sizeof(buffer), 0);
                                    }

                                }else if(estadoJugador == 4){
                                    Partida* partida = jugadorActual->partida;
                                    Jugador* contrincante;
                                    contrincante = (partida->turno % 2 == 0) 
                                        ? partida->jugador1 : partida->jugador2;
                                  

                                    if(instruccion[strlen(instruccion)-1] == '\n')
                                            instruccion[strlen(instruccion)-1] ='\0';
                                    if (strcmp(instruccion, "COLOCAR-FICHA") == 0){
                                      if(jugadorActual->sd != contrincante->sd){
                                        bzero(buffer,sizeof(buffer));
                                        strcpy(buffer,"-Err. No es tu turno\n");
                                        send(i, buffer, sizeof(buffer), 0);
                                        }else{
                                            instruccion = strtok(NULL,"\n");
                                            if(instruccion[strlen(instruccion)-1] == '\n')
                                                instruccion[strlen(instruccion)-1] ='\0';
                                            
                                            int colum=strtol(instruccion,NULL,10);
                                            int res = actualizarTablero(partida->tablero,colum,partida->turno);
                                            if(res > 0){
                                                    partida->turno=partida->turno+1;
                                                     bzero(buffer,sizeof(buffer));
                                                    strcpy(buffer,"-ok,ficha colocada\n");
                                                    send(i, buffer, sizeof(buffer), 0);
                                                    strcpy(buffer,"-tu turno\n");
                                                    send(contrincante->sd,buffer,sizeof(buffer),0);
                                                    mostrarTablero(partida->tablero,buffer);
                                                    send(contrincante->sd,buffer,sizeof(buffer),0);
                                                    if(finPartida(partida->tablero,partida->turno,res,colum)==1){

                                                         bzero(buffer,sizeof(buffer));
                                                    strcpy(buffer,"GANADOR jugador\n");
                                                    send(i, buffer, sizeof(buffer), 0);
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
                                                strcpy(buffer,"-Err. No cabe en esta columa\n");
                                                send(i, buffer, sizeof(buffer), 0);
                                            }
                                        }
                                            
                                    } else {
                                        bzero(buffer,sizeof(buffer));
                                        strcpy(buffer,"-Err. Instrucción no válida\n");
                                    }
                                }
                                /*
                                sprintf(identificador,"<%d>: %s",i,buffer);
                                bzero(buffer,sizeof(buffer));

                                strcpy(buffer,identificador);

                                printf("%s\n", buffer);

                                for(int j = 0; j < numClientes; j++)
                                    if(arrayClientes[j] != i)
                                        send(arrayClientes[j],buffer,sizeof(buffer),0);
                                */
                            }
                        }

                            /*
                                Lista *aux = *listaJugadores;
                                while(aux != NULL){
                                    bzero(buffer,sizeof(buffer));
                                    sprintf(buffer, "Nuevo Cliente conectado: %d\n",new_sd);
                                    if(aux->item->estado == 3 && aux->item->sd != new_sd){
                                        send(aux->item->sd,buffer,sizeof(buffer),0);
                                        break;
                                    }
                                    aux = aux->sig;
                                }
                             */

                        if(recibidos== 0){
                            printf("El socket %d, ha introducido ctrl+c\n", i);
                                salirCliente(buscarJugador(listaJugadores,i),&readfds,&numClientes,&listaJugadores);
                        }
                    }
                }
            }
        }
    }

	close(sd);
	return 0;
	
}

void manejador (int signum){
    printf("\nSe ha recibido la señal sigint\n");
    signal(SIGINT,manejador);
    
    //Implementar lo que se desee realizar cuando ocurra la excepción de ctrl+c en el servidor
}
