#include "netstruct.h"
#include "tcp.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
int UDPconnect(char* regIP, char* regUDP){
	int fd, errcode;
	ssize_t n;
	socklen_t addrlen;
	struct addrinfo hints,*res;
	struct sockaddr_in addr;
	fd=socket(AF_INET,SOCK_DGRAM,0); //UDP socket
	if(fd==-1)
		/*error*/exit(1);
	memset(&hints,0,sizeof hints);
	hints.ai_family=AF_INET; //IPv4
	hints.ai_socktype=SOCK_DGRAM; //UDP socket
	errcode=getaddrinfo(regIP,regUDP,&hints,&res);
	if(errcode!=0)
		/*error*/ exit(1);
	if(n==-1) /*error*/ exit(1);
	addrlen=sizeof(addr);
	if(n==-1)
		/*error*/ exit(1);
	return fd;
}

bool UDPquery (netnode *host, char *net){
	fprintf(fdopen(host->UDPsocket, "w"),"NODES" , ...);

}

bool UDPreg(netnode *host, char *net, char *id){
	bool regflag=0;
	char *buffer[128];
	int id_int;
	int id_first;
	id_int=atoi(id);
	id_first=id_int;
	while (regflag==0){
		fprintf(fdopen(host->UDPsocket, "w"), "REG %s %s %s %s %s\n", net, id, host->self.IP, host->self.TCPport);
		fscanf(fdopen(host->UDPsocket,"r"),"%s", buffer);
		if(strcmp(buffer, "OKREG\n")==0){
			regflag=1;
			host->self.id=id;
			return 1;
		}
		else{
			id_int=atoi(id);
			id_int++;
			if(id_int==100)
				id_int=0;
			else if(id_int==id_first)
				return 0;
			sprintf(id, "%d",id_int);

		
						
		}
	
	
	}
}
