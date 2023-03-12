#include "netstruct.h"
#include "udp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

int UDPconnect(char* regIP, char* regUDP){
	int fd, errcode;
	ssize_t n;
	struct addrinfo hints;
	struct addrinfo *res;
 	struct sockaddr_in addr;

	socklen_t addrlen;
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

entry* UDPquery (netnode *host, char *net,char* regIP, char* regUDP){
	entry *data=(entry*)malloc(sizeof(entry));
	if(data==NULL){
		printf("ERROR[000]: Out of memory, closing...");
		exit(1);
	}
	char buffer[99*128];
 	struct addrinfo hints;
	struct addrinfo *res;
 	struct sockaddr_in addr;
	socklen_t addrlen=sizeof(addr);
	int n_lines = 0;
	int chosen_line = 0;
	int aux = 0; 
	int errcode=getaddrinfo(regIP,regUDP,&hints,&res);
	if(errcode!=0) /*error*/ exit(1);
	int fd =host->UDPsocket;
	srand(time(NULL)); // seed the random number generator
	sprintf(buffer, "NODES %s\n", net);
	printf("host:%s\n", buffer);
	
	sendto(host->UDPsocket, buffer, strlen(buffer),0,res->ai_addr,res->ai_addrlen);	
	recvfrom(fd, buffer, 14,0,(struct sockaddr*)&addr, &addrlen);


	// Determine file size
	off_t size = lseek(fd, 0, SEEK_END);
	
	if (size ==1)
		return NULL;
	printf("size: %ld\n", size);
	printf("%s\n", buffer);
	// Generate random position within file
	srand(time(NULL));
	size=size-2;
	off_t pos = (rand() % size) + 2;

	// Read line of text starting from random position
	ssize_t n = recvfrom(fd, buffer, sizeof(buffer),0,(struct sockaddr*)&addr, &addrlen);
	printf("Server: %s\n",buffer);
	if (n < 0) {
		printf("Failed to read from file\n");
		exit(1);
	}

	// Find end of line
	char *end = buffer;
	while (*end != '\n' && end < buffer + n) {
		end++;
	}
	*end = '\0';
	printf("HOST: Chose data: %s\n",buffer);
	sscanf(buffer, "%s %s %s", data->id, data->IP, data->TCPport);	
	return data;
	
}

bool UDPreg(netnode *host, char *net, char *id,char* regIP, char* regUDP){
	bool regflag=0;
	char message[128];
	char *buffer=(char*)malloc(6*sizeof(char));
	struct addrinfo hints;
	struct addrinfo *res;
 	struct sockaddr_in addr;
	int id_int;
	int errcode=getaddrinfo(regIP,regUDP,&hints,&res);
	if(errcode!=0) /*error*/ exit(1);
	int id_first;
	FILE *stream;
	socklen_t addrlen=sizeof(addr);
	id_int=atoi(id);
	id_first=id_int;
	printf("entrei no registo\n");
	while (regflag==0){
		printf("UDP socket:%d\n",host->UDPsocket);
		sprintf(message,"REG %s %s %s %s\n", net, id, host->self.IP, host->self.TCPport);
		
		//printf("%s %d\n", buffer, strlen(buffer));
		printf("%s",id);
		printf("%s",id);
		sendto(host->UDPsocket, message,strlen(message),0, res->ai_addr,res->ai_addrlen);


		printf("registei\n");
		int n=recvfrom(host->UDPsocket, buffer, 6,0,(struct sockaddr*)&addr, &addrlen);
		printf("servidor: %s\n",buffer);
		if(strcmp(buffer, "OKREG")==0){
			printf("O Server aceitou\n");
			regflag=1;
			host->self.id=id;
			free(buffer);
			return 0;
		}
		else{
			id_int=atoi(id);
			id_int++;
			if(id_int==100)
				id_int=0;
			else if(id_int==id_first){
				free(buffer);
				return 1;
			}
			sprintf(id, "%d",id_int);

		
						
		}
	
	
	}
	return 1;
}
