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
	sigaction(SIGPIPE, &(struct sigaction){SIG_IGN}, NULL);
	struct netnode *host=NULL;
	ssize_t n;
	socklen_t addrlen;
	struct addrinfo hints, *res=NULL;
	struct sockaddr_in addr;
	char buffer[128];
	int fd, errcode, newfd, afd = 0;
	fd_set rfds;
	enum{ idle, busy } state=idle;
	int maxfd, counter;

	char *regUDP, *regIP, *tcp_port, *IP;
	char backup_regIP[16]="193.136.138.142", backup_regUDP[6]="59000";
	if( argc < 3 || argc > 5){
		printf("ERROR [000]: Faulty init. Missing or Overflowing parameters.\n");
		exit(EXIT_FAILURE);
	}


	IP=argv[1];
	tcp_port=argv[2];
	printf("Eu sou o %s na porta %s\n ",IP, tcp_port);

	if (argc >= 4)
		regIP=argv[3];
	else
		regIP=backup_regIP;
	if (argc == 5)
		regUDP=argv[4];

	else
		regUDP=backup_regUDP;
	printf("O meu servidor é %s na porta %s \n", regIP, regUDP);
	host=(netnode*)malloc(sizeof(netnode));
	host->self.IP=IP;
	host->self.TCPport=tcp_port;
	host->self.UDPport=regUDP;

	host->TCPsocket=setTCP_server(tcp_port, fd, errcode, n, addrlen, hints, res, addr, buffer);
	printf("Socket TCP: %d\n", host->TCPsocket);

	host->UDPsocket=UDPconnect(regIP, regUDP);
	printf("Socket UDP: %d\n", host->UDPsocket);
	host->serverIP=regIP;
	host->serverUDP=regUDP;
	while (1){
		FD_ZERO (&rfds);
		printf("entrei no while\n");
		FD_SET(0,&rfds);
		FD_SET(host->TCPsocket, &rfds);
		maxfd=host->TCPsocket;
		/*tenho que dar fix disto*/
		counter =select (maxfd + 1, &rfds, (fd_set *) NULL, (fd_set *) NULL,(struct timeval *) NULL);
		printf("counter: %d\n", counter);
		if (counter <= 0)
		/*error */ exit (1);

		while(counter>0){
			if (FD_ISSET (host->TCPsocket, &rfds)){
				newfd=handshake(host, hints, res, addr,buffer,rfds);
				/* if(newfd = -1){	
					afd = newfd;
					state = busy;
			
				}*/
			}

			if (FD_ISSET (0, &rfds)){
				fgets(buffer, 128 , stdin);
				proc_stdin(buffer, host);
			//	state=busy;
			}
			
		/*switch (state){
		  case idle:
		    break;

		    //e dar fix daqui para baixo
		  case busy:
		    if (FD_ISSET (host->TCPsocket, &rfds)){
			FD_CLR (fd, &rfds);
			addrlen = sizeof (addr);
			if ((newfd = accept (fd, &addr, &addrlen)) == -1)
			  //error 
			  //exit (1);
			close (newfd);
		      }
		    else if (FD_ISSET (afd, &rfds) && fd != 0){
			FD_CLR (afd, &rfds);
			if ((n = read (afd, buffer, 128)) != 0){
			    if (n == -1)
			      //error  exit (1);
			    // ... write buffer in afd
			  }
			else{
			    close (afd);
			    state = idle;
			}		//connection closed by peer
		      }
		    break;
		  }//switch(state)	
		counter--;	
		*/
		}
	}//while(1)
	freeaddrinfo(res);
	close(fd);
	printf("end");
	return 0;
}
