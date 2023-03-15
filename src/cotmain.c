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
	sigaction(SIGPIPE, &((struct sigaction){SIG_IGN}), NULL);
	struct netnode *host=NULL;
	ssize_t n=0;
	socklen_t addrlen=0;
	struct addrinfo hints, *res=NULL;
	struct sockaddr_in addr;
	char buffer[128];
	int fd=0, errcode=0, newfd=0, afd = 0;
	fd_set rfds;
	int maxfd=0, counter=0;

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
	if(!Is_ValidIPv4(IP)|| !Is_ValidPort(tcp_port)){
		printf("ERROR[000]: Incorrect Host-Client data. Check host IPv4 and TCP port.");
		exit(1);
	}
	if(!Is_ValidIPv4(regIP)|| !Is_ValidPort(regUDP)){
		printf("ERROR[000]: Incorrect Regitrar Server data. Check Server IPv4 and UDP port.");
		exit(1);
	}
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
			
			}
	}//while(1)
	freeaddrinfo(res);
	close(fd);
	printf("end");
	return 0;
}
