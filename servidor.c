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


#define MSG_SIZE 250
#define MAX_CLIENTS 50

void manejador(int signum);
void salirCliente(int socket, fd_set * readfds, int * numClientes, int arrayClientes[]);



int main ( )
{
    int sd, new_sd;
    struct sockaddr_in sockname, from;
    char buffer[MSG_SIZE];
    socklen_t from_len;
    fd_set readfds, auxfds;
    int salida;
    int arrayClientes[MAX_CLIENTS];
    int numClientes = 0;
    int recibidos;
    char identificador[MSG_SIZE];
    
    int on, ret;


  	sd = socket (AF_INET, SOCK_STREAM, 0);
	if (sd == -1)
	{
		perror("No se puede abrir el socket cliente\n");
    		exit (1);	
	}


    on=1;
    ret = setsockopt( sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

	sockname.sin_family = AF_INET;
	sockname.sin_port = htons(2060);
	sockname.sin_addr.s_addr = INADDR_ANY;

	if (bind (sd, (struct sockaddr *) &sockname, sizeof (sockname)) == -1)
	{
		perror("Error en la operación bind");
		exit(1);
	}
	
	from_len = sizeof (from);


	if(listen(sd,1) == -1){
		perror("Error en la operación de listen");
		exit(1);
	}
    
    FD_ZERO(&readfds);
    FD_ZERO(&auxfds);
    FD_SET(sd,&readfds);
    FD_SET(0,&readfds);

    signal(SIGINT,manejador);
    
    while(1){        
        auxfds = readfds;
        salida = select(FD_SETSIZE,&auxfds,NULL,NULL,NULL);
        
        if(salida > 0){
            for(int i=0; i<FD_SETSIZE; i++){
                if(FD_ISSET(i, &auxfds)) {
                    if( i == sd){
                        if((new_sd = accept(sd, (struct sockaddr *)&from, &from_len)) == -1){
                            perror("Error aceptando peticiones");
                        }
                        else
                        {
                            if(numClientes < MAX_CLIENTS){
                                arrayClientes[numClientes] = new_sd;
                                numClientes++;
                                FD_SET(new_sd,&readfds);
                            
                                strcpy(buffer, "Bienvenido al chat\n");
                                send(new_sd,buffer,sizeof(buffer),0);
                            
                                for(int j=0; j<(numClientes-1);j++){
                                    bzero(buffer,sizeof(buffer));
                                    sprintf(buffer, "Nuevo Cliente conectado: %d\n",new_sd);
                                    send(arrayClientes[j],buffer,sizeof(buffer),0);
                                }
                            }
                            else{
                                bzero(buffer,sizeof(buffer));
                                strcpy(buffer,"Demasiados clientes conectados\n");
                                send(new_sd,buffer,sizeof(buffer),0);
                                close(new_sd);
                            }
                        }
                    }
                    else if (i == 0){
                        bzero(buffer, sizeof(buffer));
                        fgets(buffer, sizeof(buffer),stdin);
                        
                        if(strcmp(buffer,"SALIR\n") == 0){
                            for (int j = 0; j < numClientes; j++){
                                bzero(buffer, sizeof(buffer));
                                strcpy(buffer,"Desconexión servidor\n"); 
                                send(arrayClientes[j],buffer , sizeof(buffer),0);
                                close(arrayClientes[j]);
                                FD_CLR(arrayClientes[j],&readfds);
                            }
                            close(sd);
                            exit(-1);
                            
                        }
                    } 
                    else{
                        bzero(buffer,sizeof(buffer));
                        recibidos = recv(i,buffer,sizeof(buffer),0);
                    
                        if(recibidos > 0){
                            if(strcmp(buffer,"SALIR\n") == 0){
                                salirCliente(i,&readfds,&numClientes,arrayClientes);
                            }
                            else{
                                sprintf(identificador,"<%d>: %s",i,buffer);
                                bzero(buffer,sizeof(buffer));

                                strcpy(buffer,identificador);

                                printf("%s\n", buffer);

                                for(int j = 0; j < numClientes; j++)
                                    if(arrayClientes[j] != i)
                                        send(arrayClientes[j],buffer,sizeof(buffer),0);
                            }
                        }
                        if(recibidos== 0){
                            printf("El socket %d, ha introducido ctrl+c\n", i);
                            salirCliente(i,&readfds,&numClientes,arrayClientes);
                        }
                    }
                }
            }
        }
    }

	close(sd);
	return 0;
	
}

void salirCliente(int socket, fd_set * readfds, int * numClientes, int arrayClientes[]){
  
    char buffer[250];
    int j;
    
    close(socket);
    FD_CLR(socket,readfds);
    
    for (j = 0; j < (*numClientes) - 1; j++)
        if (arrayClientes[j] == socket)
            break;
    for (; j < (*numClientes) - 1; j++)
        (arrayClientes[j] = arrayClientes[j+1]);
    
    (*numClientes)--;
    
    bzero(buffer,sizeof(buffer));
    sprintf(buffer,"Desconexión del cliente: %d\n",socket);
    
    for (j = 0; j<(*numClientes); j++)
        if(arrayClientes[j] != socket)
            send(arrayClientes[j],buffer,sizeof(buffer),0);


}


void manejador (int signum){
    printf("\nSe ha recibido la señal sigint\n");
    signal(SIGINT,manejador);
    
    //Implementar lo que se desee realizar cuando ocurra la excepción de ctrl+c en el servidor
}
