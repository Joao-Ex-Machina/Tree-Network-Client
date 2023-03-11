#include "netstruct.h"
#include "udp.h"
#include <stdio.h>
#include <stdlib.h>

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

entry* UDPquery (netnode *host, char *net){
	entry *data=NULL;
	if(data==NULL){
		printf("ERROR[000]: Out of memory, closing...");
		exit(1);
	}
	char line[128];
	int n_lines = 0;
	int chosen_line = 0;
		srand(time(NULL)); // seed the random number generator
	fprintf(fdopen(host->UDPsocket, "w"),"NODES %s" , net);

		// count the number of lines in stdin
		while ((fgets(line, sizeof(line), fdopen(host->UDPsocket, "r"))) != NULL)
			n_lines++;
		if(n_lines==1) //read NODELIST\n line only
			return data;
		// reset the file pointer to the beginning of stdin
		fseek(fdopen(host->UDPsocket, "r"), 0, SEEK_SET);

		// choose a random line
		n_lines = n_lines - 2;
		if (n_lines >= 0) {
		chosen_line = (rand() % n_lines) + 2;
		}

		// extract the chosen line and print it
		int current_line = 0;
		while (fgets(line, sizeof(line), stdin) != NULL) {
			if (current_line == chosen_line) {
				data=(entry *)malloc(sizeof(entry));
				data->IP=(char*)malloc(INET_ADDRSTRLEN);
				data->TCPport=(char*)malloc(6*sizeof(char));
				data->id=(char*)malloc(3*(sizeof(char)));
				sscanf(line,"%s %s %s\n",data->id, data->IP, data->TCPport);
				break;
			}
			current_line++;
		}
		return data;


}

bool UDPreg(netnode *host, char *net, char *id){
	bool regflag=0;
	char *buffer=(char*)malloc(6*sizeof(char));
	int id_int;
	int id_first;
	id_int=atoi(id);
	id_first=id_int;
	while (regflag==0){
		fprintf(fdopen(host->UDPsocket, "w"), "REG %s %s %s %s ", net, id, host->self.IP, host->self.TCPport);
		fscanf(fdopen(host->UDPsocket,"r"),"%s", buffer);
		if(strcmp(buffer, "OKREG")==0){
			regflag=1;
			host->self.id=id;
			free(buffer);
			return 1;
		}
		else{
			id_int=atoi(id);
			id_int++;
			if(id_int==100)
				id_int=0;
			else if(id_int==id_first){
				free(buffer);
				return 0;
			}
			sprintf(id, "%d",id_int);

		
						
		}
	
	
	}
	return 1;
}
