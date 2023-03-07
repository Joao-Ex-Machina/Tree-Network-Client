#include "main.h"
#include "tcp.h"
#include "io.h"
#include "netstruct.h"
#include <stdio.h>
#include <stdlib.h>

int main (int argc, char *argv[]){
  struct netnode *host=NULL, *aux=NULL;
  ssize_t n;
  socklen_t addrlen;
  struct addrinfo hints, *res;
  struct sockaddr_in addr;
  char buffer[128];
  int fd, errcode, newfd, afd = 0;
  fd_set rfds;
  enum{ idle, busy } state;
  int maxfd, counter;
//      int regIP = [193,136,138,142];
  char regUDP[6] = "59000";
  char regIP[16]="193.136.138.142";
  char tcp_port[6] = "58001";
  char IP[16]= "1.1.1.1";

/*	if( argc < 3 || argc > 5){
        	printf("Por favor verifique se usou todos os dados necessários e tente outra vez.\n");
        	exit(EXIT_FAILURE);
    	}

    	int main_IP = inet_aton(argv[1]);
    	tcp_port = strtol(argv[2], NULL, 10);

    	if (argc >= 4){
        	regIP =  inet_aton(argv[3]);
		if (argc == 5)
        		regUDP =  strtol(argv[4], NULL, 10);
i    	}
*/
host=(netnode*)malloc(sizeof(netnode));

host->TCPsocket=setTCP_server(tcp_port, fd, errcode, n, addrlen, hints, res, addr, buffer);
aux=host;
//host->UDPsocket=setUDP_server;

while (1){
	/*tenho que dar fix disto*/
      FD_ZERO (&rfds);
      switch (state){
	case idle:
	  FD_SET (fd, &rfds);
	  maxfd = fd;
	  break;
	case busy:
	  FD_SET (fd, &rfds);
	  FD_SET (afd, &rfds);
	  maxfd = max (fd, afd);
	  break;
	}			//switch(state)
      counter =select (maxfd + 1, &rfds, (fd_set *) NULL, (fd_set *) NULL,(struct timeval *) NULL);
      if (counter <= 0)
	/*error */ exit (1);

      while(counter>0){
	switch (state){
	  case idle:
	    if (FD_ISSET (host->TCPsocket, &rfds)){
		FD_CLR (host->TCPsocket, &rfds);
		addrlen = sizeof (addr);
		if ((newfd = accept (host->TCPsocket, &addr, &addrlen)) == -1)
		  /*error */ exit (1);
		fgets(buffer,128,newfd);

		/* A FAZER Processar mensagem de tipo NEW*/
		/*idk man maybe meter o Jorge a fazer isto*/

		if(aux->interns==NULL){
			aux->intern=(entry*)malloc(sizeof(entry));
		}
		else{
			while(aux->interns->brother!=NULL){
				aux->interns=aux->interns->brother;

			}

			aux->interns->brother=(entry*)malloc(sizeof(entry));
			aux->interns=aux->interns->brother;
		}

		aux->interns->IP=;
		aux->interns->id=;
		aux->interns->TCPport=;
		aux->interns->fd=newfd; /*Passar por referência*/
		aux=host;
		/* Caso host->backup.id == host->self.id*/
		/*Copiar estes aux->interns todos para o backup*/

		fprintf(newfd, "EXTERN \s \s \s\n", host->external.id, host->external.IP, host->external.TCPport);
		afd = newfd;
		state = busy;
	      }
	    else if (FD_ISSET (0, &rfds)){
			fgets(buffer, 128 , stdin);
			proc_stdin(buffer);
			//state=busy;
	    }

	    break;
	    /*e dar fix daqui para baixo*/
	  case busy:
	    if (FD_ISSET (fd, &rfds)&& fd !=0 ){
		FD_CLR (fd, &rfds);
		addrlen = sizeof (addr);
		if ((newfd = accept (fd, &addr, &addrlen)) == -1)
		  /*error */ exit (1);
		close (newfd);
	      }
	    else if (FD_ISSET (afd, &rfds) && fd != 0){
		FD_CLR (afd, &rfds);
		if ((n = read (afd, buffer, 128)) != 0){
		    if (n == -1)
		      /*error */ exit (1);
		    /* ... write buffer in afd */
		  }
		else{
		    close (afd);
		    state = idle;
		}		//connection closed by peer
	      }
	    break;
	  }//switch(state)	
  	counter--;	
      }
  }//while(1)
   	freeaddrinfo(res);
   	close(fd);
	printf("end");
	return 0;
}
