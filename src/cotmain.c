/*-----------------------------------------------------------------------------------------------------+
| cotmain.c              | Main routine for the cot program (Tree Network Client). Includes inits,     |
|                        | while (1) routine with select and File descriptor dirty bit set and clear.  |
|                        |                                                                             |
+------------------------------------------------------------------------------------------------------+
| Authors: Joao Barreiros C. Rodrigues nº99968, Jorge Miguel Monteiro Contente nº102143                |
|          LEEC-IST                                                                                    |
| Date: March-April 2023                                                                               |
+-----------------------------------------------------------------------------------------------------*/
#include "main.h"
#include "tcp.h"
#include "udp.h"
#include "io.h"
#include "netstruct.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <signal.h>
int main (int argc, char *argv[]){
	struct sigaction act = {.sa_handler = SIG_IGN};
    	if (sigaction(SIGPIPE, &act, NULL) == -1)
        	exit(-1);
	struct netnode *host=NULL;
	struct sockaddr_in addr;
	struct addrinfo hints, *res=NULL;
	char buffer[128];
	struct entry *aux=NULL, *aux2=NULL;
	int newfd=0;
	int maxfd=0, counter=0;

	char *regUDP, *regIP, *tcp_port, *IP;
	char backup_regIP[16]="193.136.138.142", backup_regUDP[6]="59000";
	if( argc < 3 || argc > 5){
		printf("ERROR [000]: Faulty init. Missing or Overflowing parameters.\n");
		exit(EXIT_FAILURE);
	}


	IP=argv[1];
	tcp_port=argv[2];
	//printf("Eu sou o %s na porta %s\n ",IP, tcp_port);

	if (argc >= 4)
		regIP=argv[3];
	else
		regIP=backup_regIP;
	if (argc == 5)
		regUDP=argv[4];

	else
		regUDP=backup_regUDP;
	/*check info validity*/
	if(!Is_ValidIPv4(IP)|| !Is_ValidPort(tcp_port)){
		printf("ERROR[000]: Incorrect Host-Client data. Check host IPv4 and TCP port.");
		exit(1);
	}
	if(!Is_ValidIPv4(regIP)|| !Is_ValidPort(regUDP)){
		printf("ERROR[000]: Incorrect Regitrar Server data. Check Server IPv4 and UDP port.");
		exit(1);
	}
	//printf("O meu servidor é %s na porta %s \n", regIP, regUDP);
	/*QUICK HOST INITIALIZATION*/
	host=(netnode*)malloc(sizeof(netnode));
	host->TCPsocket=-1;
	host->self.IP=IP;
	host->self.TCPport=tcp_port;
	host->self.UDPport=regUDP;
	host->is_connected=false;
	host->serverIP=regIP;
	host->serverUDP=regUDP;
	host->external.IP=NULL;
	host->external.fd=-1;
	host->interns=NULL;
	host->content_list=NULL;
	host->routing_list=NULL;	
	/*SOCKET INIT*/
	//printf("Socket TCP: %d\n", host->TCPsocket);
	host->UDPsocket=UDPconnect(regIP, regUDP);
	//printf("Socket UDP: %d\n", host->UDPsocket);
	while (1){
		/*RESET all fd dirty bits to 0*/
		FD_ZERO (&(host->rfds));
		//printf("entrei no while\n");
		
		FD_SET(0,&(host->rfds)); /*stdin*/
		maxfd=0;

		/*Check and set dirty bits for listening socket and connected nodes*/
		if(host->TCPsocket!=-1){
			FD_SET(host->TCPsocket, &(host->rfds));
			if(host->TCPsocket>maxfd)
				maxfd=host->TCPsocket;
		}
		if(host->external.fd !=-1){
			FD_SET(host->external.fd, &(host->rfds));
			if(host->external.fd>maxfd)
				maxfd=host->external.fd;
		}
		aux=host->interns;
	//	printf("%p",(void*)host->interns);
		
		while(aux!=NULL){
			//printf("Tenho internos não nulos");
			FD_SET(aux->fd, &(host->rfds));
			if(aux->fd>maxfd)
				maxfd=aux->fd;

			aux=aux->brother;

		}
		
		counter =select (maxfd + 1, &(host->rfds), (fd_set *) NULL, (fd_set *) NULL,(struct timeval *) NULL);
	//	printf("counter: %d\n", counter);
		if (counter <= 0){
			printf("[ERROR]: FATAL--INVALID FD SET! ABORTING!\n");
			leave(host);
		/*error */ exit (1);
		}
		while(counter>0){
			/*Check for dirty bits. Rewire to respective procedures*/
			if (FD_ISSET (host->TCPsocket, &(host->rfds))){
				//printf("alguém quer-se registar\n");
				newfd=handshake(host, hints, res, addr,buffer,host->rfds);
				newfd ++;
				newfd --;
				FD_CLR(host->TCPsocket, &(host->rfds));
			}

			if (FD_ISSET (host->external.fd, &(host->rfds))){
				if(strcmp(host->external.id, host->self.id)!=0){
					FD_CLR(host->external.fd, &(host->rfds));
					//printf("O externo apitou");
					proc_extern(host);
					
				}

				//else
					//printf("Estou sozinho na rede");	
					
			}

			aux=host->interns;
			aux2=host->interns;
			while(aux!=NULL){
				if(FD_ISSET (aux->fd, &(host->rfds))){
					FD_CLR(aux->fd, &(host->rfds));
				//	printf("Um interno (%s) apitou", aux->id);
					aux=proc_intern(host, aux, aux2);
				}
				if(aux!=NULL){
					aux2=aux;
					aux=aux->brother;

				}
							
			}

			if (FD_ISSET (0,&(host->rfds))){
				fgets(buffer, 128 , stdin);
				FD_CLR(0, &(host->rfds));
				proc_stdin(buffer, host);
				fflush(stdin);
			//	state=busy;
			}
			counter--;
		}
	}//while(1)
	/*WILL NEVER REACH THIS*/
	freeaddrinfo(res);
	printf("end");
	return 0;
}
