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
	int fd=0, errcode=0;
	ssize_t n=0;
	struct addrinfo hints={.ai_family = AF_INET, .ai_socktype = SOCK_STREAM};
	struct addrinfo *res=NULL;
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
	n=sizeof(addr);
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
	char *buffer=(char*)malloc(128*sizeof(char));
	char *connections[99];
	for (int i=0; i<99; i++){
		connections[i]=(char*)malloc(128*sizeof(char));
	}
	char *buffercontrol=(char*)malloc(128*sizeof(char));
 	char *chosen_buffer;
	struct addrinfo hints={.ai_family = AF_INET, .ai_socktype = SOCK_STREAM};
	struct addrinfo *res;
 	struct sockaddr_in addr;
	socklen_t addrlen=sizeof(addr);
	int n=0,n_lines = 0;
	int chosen_line = 0; 
	int errcode=getaddrinfo(regIP,regUDP,&hints,&res);
	if(errcode!=0) /*error*/ exit(1);
	int fd =host->UDPsocket;
	srand(time(NULL)); // seed the random number generator
	sprintf(buffer, "NODES %s\n", net);
	sprintf(buffercontrol, "NODESLIST %s\n", net);
	printf("HOST:%s\n", buffer);
	
	sendto(host->UDPsocket, buffer, strlen(buffer),0,res->ai_addr,res->ai_addrlen);	
	recvfrom(fd, buffer, 128,0,(struct sockaddr*)&addr, &addrlen);
	printf("SERVER: %s\n", buffer);
	buffercontrol=strtok(buffer, "\n");
	connections[0]=buffercontrol;
		while(connections[n]!=NULL){
			if (n > 5)
				break;
			n++;

			connections[n]=strtok(NULL, "\n");
				//free(aux);
		}

	printf("SERVER:%s\n",buffer);
	if(strcmp(buffer, buffercontrol)!=0){
		printf("[ERROR]: WRONG FORMAT ON REGISTRAR SERVER, EXITING...");
		exit(1);
	}
	n--;
	chosen_line=(rand()%n)+1;
	if(n==0){ //network is empty
		data->id=host->self.id;
		data->IP=host->self.IP;
		data->TCPport=host->self.TCPport;
		free(buffer);
		return data;

	}
	chosen_buffer=connections[chosen_line];
	printf("HOST: Chose data:[%s]\n",chosen_buffer);
	printf("banan: %d\n",sscanf(chosen_buffer, "%2c %s %s", data->id, data->IP, data->TCPport));
	printf("saved: %s %s %s\n", data->id, data->IP, data->TCPport);
	if(sscanf(chosen_buffer, "%s %s %s", data->id, data->IP, data->TCPport) != 5){
		printf("[ERROR]: WRONG FORMAT ON REGISTRAR SERVER, EXITING...");
		free(buffer);
		exit(1);

	}	
	free(buffer);
	return data;
	
}

bool UDPreg(netnode *host, char *net, char *id,char* regIP, char* regUDP){
	bool regflag=0;
	char message[128];
	char *buffer=(char*)malloc(6*sizeof(char));
	struct addrinfo hints={.ai_family = AF_INET, .ai_socktype = SOCK_STREAM};
	struct addrinfo *res=NULL;
 	struct sockaddr_in addr;
	int id_int=0;
	int errcode=getaddrinfo(regIP,regUDP,&hints,&res);
	if(errcode!=0) /*error*/ exit(1);
	int id_first=0;
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
		recvfrom(host->UDPsocket, buffer, 6,0,(struct sockaddr*)&addr, &addrlen);
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
