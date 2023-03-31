#include "netstruct.h"
#include "tcp.h"
#include "udp.h"
#include "content.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int setTCP_server (char *tcp_port){
	int fd;
	int errcode;
	ssize_t n;
	addrinfo hints;
	addrinfo * res;

	fd = socket (AF_INET, SOCK_STREAM, 0);	//TCP socket
	if (fd == -1)
		exit(1);			/*error */
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;	//IPv4
	hints.ai_socktype = SOCK_STREAM;	//TCP socket
	hints.ai_flags = AI_PASSIVE;
	errcode = getaddrinfo (NULL, tcp_port, &hints, &res);
	if((errcode) != 0)
		exit (1);			/*error */
	n = bind (fd, res->ai_addr, res->ai_addrlen);
	if(n == -1)
		exit (1);			/*error */
	if(listen (fd, 5) == -1)
		exit (1); /*error */
	return fd;
}

void djoin (char* net, char* id, char* bootid, char* bootIP, char* bootTCP, netnode* node){
	int fd=0,errcode=0,i=0;
	ssize_t n=0;
	struct addrinfo hints,*res=NULL;
	char *buffer=(char*)malloc(128*sizeof(char));
	memset(buffer, '\0', 128*sizeof(char));
	char* token[4];
	//printf("%s %s\n",id,bootid);
	//printf("ENTREI NO DJOIN\n");
	if(node->is_connected){
		printf("[FAULT]: You have already joined a network.\n Please use leave before joining a new net\n");
		return;
	}
	node->self.id=id;	
	node->net=net;
	for (i=0; i<4; i++)
		token[i]=(char*)malloc(128*(sizeof(char)));
	//printf("acabei setup\n");
	//printf("comparei os dois : %d \n", strcmp(id, bootid));
	if(strcmp(id, bootid)==0){
		printf("INFO[000]: SELF CONNECTION\n");
		bootIP=node->self.IP;
		bootTCP=node->self.TCPport;
		node->backup.IP=bootIP;
		node->backup.TCPport=bootTCP;
		node->backup.id=bootid;
		node->external.fd=fd; /*e por hoje é tudo, vou dormir*/
		node->external.IP=bootIP;
		node->external.TCPport=bootTCP;
		node->external.id=bootid;
		node->is_connected=true;
		node->TCPsocket=setTCP_server(node->self.TCPport);
		//printf("A sair do djoin\n");
		return;
	}

	fd=socket(AF_INET,SOCK_STREAM,0); //TCP socket
	printf("%d\n", fd);
	if (fd==-1)
		exit(1); //error
	memset(&hints,0,sizeof hints);
	hints.ai_family=AF_INET; //IPv4
	hints.ai_socktype=SOCK_STREAM; //TCP socket
	errcode=getaddrinfo(bootIP,bootTCP,&hints,&res);
	//printf("Vou ligar-me ao %s %s\n", bootIP, bootTCP);

	if(errcode!=0){
		printf("[ERROR]: Something went wrong :/ ABORTING!");
		/*error*/exit(1);
	}
	if (strcmp(id, bootid)!=0){
		node->TCPsocket=setTCP_server(node->self.TCPport);
		n=connect(fd,res->ai_addr,res->ai_addrlen);
		if(n==-1){
			printf("ERROR: Cannot Connect to node or network. Please clear this network or choose a new one.");
			node->is_connected=true;
			close(fd);
			leave(node);
			return;
		}
		memset(buffer, '\0', 128*sizeof(char));
		sprintf(buffer,"NEW %s %s %s\n",node->self.id, node->self.IP ,node->self.TCPport);
		n=write(fd, buffer, strlen(buffer));
		//printf("Mandei isto: %s\n", buffer);
		if(n==-1)
			/*error*/exit(1);
		memset(buffer, '\0', 128*sizeof(char));
		n=read(fd,buffer,128);
		
		while(buffer[strlen(buffer)-1]!='\n'){
			if(n==0||n==-1||(strcmp(buffer,"\0")==0))
				break;
			else
				n=read(fd,buffer,128);
			if(n>=128)
				break;
			sleep(1);	
		}

		if(n==-1)
			/*error*/exit(1);
		if(n==0 || (strcmp(buffer,"\0")==0)){
			printf("ERROR: Cannot Connect to node or network. Please clear this network or choose a new one.");
			node->is_connected=true;
			leave(node);
			return;
		}
		printf("O tipo disse mesmo: %s\n",buffer);
		/*processar EXTERN*/
		i=0;
		buffer=strtok(buffer,"\n");
		token[0]=strtok(buffer, " ");
		while(token[i]!=NULL){
			if (i > 3)
				break;
			i++;
			token[i]=strtok(NULL, " ");
		}
		if(strcmp(token[0], "EXTERN")!=0){
			node->is_connected=true;
			leave(node);
			return;

		}
	//printf("I got this: %s %s %s\n", token[1], token[2], token[3]);	
	node->backup.id=token[1];
	node->backup.IP=token[2];
	node->backup.TCPport=token[3];
	if(n==-1)
		/*error*/exit(1);
	}

	node->external.fd=fd; /*e por hoje é tudo, vou dormir*/
	node->external.IP=bootIP;
	node->external.TCPport=bootTCP;
	node->external.id=bootid;
	add_neighbour(node, bootid, bootid,fd);
	node->is_connected=true;
	// printf("A sair do djoin\n");

	return;

}

int handshake(netnode *host,addrinfo hints, addrinfo *res, sockaddr_in addr,char *buffer,fd_set rfds){
	char* token[4]; /*função*/
	char message[128];
	int newfd=0,i=0;
	char* buffer2 = (char*)malloc(128*sizeof(char)); // dup buffer to avoid writing on top of other data. Will probably fix in another version
	netnode* aux=host;
	entry* aux2=host->interns, *aux3=NULL;
			//printf("ENTER HANDSHAKE\n");
			for (i=0; i<4; i++)
				token[i]=(char*)malloc(128*(sizeof(char)));
			FD_CLR(host->TCPsocket, &rfds);
			socklen_t addrlen = sizeof (addr);
			if ((newfd = accept (host->TCPsocket, (struct sockaddr*)&addr, &addrlen)) == -1)
			  return newfd;
			int n=read(newfd, buffer2, 128);
			if(buffer2[strlen(buffer2)-1]!='\n'){	
				while(buffer2[strlen(buffer2)-1]!='\n'){
					if(n==0||n==-1||(strcmp(buffer2,"\0")==0))
						break;
					else
						n=read(newfd,buffer2,128);
					if(n>=128)
						break;
					sleep(1);	
				}
			}
			if(n==0||n==-1||(strcmp(buffer2,"\0")==0)){
				printf("OUTSIDE CLIENT FAILED TO CONNECT\n");
				return -1;

			}
	
			i=0;
			buffer2=strtok(buffer2, "\n");
			token[i]=strtok(buffer2, " ");
			while(token[i]!=NULL){
				if (i > 3)
					break;
				i++;
				token[i]=strtok(NULL, " ");
			}
			
			if(token[0]==NULL){
				printf("WRONG FORMAT ON OUTSIDE CLIENT\n");
				return -1;

			}
			if(strcmp(token[0], "NEW")!=0){
				printf("WRONG FORMAT ON OUTSIDE CLIENT\n");
				for (int i=0; i<3; i++){
					free(token[i]);
					return -1;
				
				}
			}
			
						/* IF host->external.id == host->self.id => I AM ALONE*/
			/*THIS CONNECTIONS WILL BECOME MY EXTERNAL. A NEW ANCHOR!*/
			if(strcmp(host->external.id, host->self.id)==0){
				host->external.IP=token[2];
				aux->external.id=token[1];
				aux->external.TCPport=token[3];
				aux->external.fd=newfd;

			}
			else{
				if(aux2==NULL){
					aux2=(entry*)malloc(sizeof(entry));
					host->interns=aux2;
				}
				else{
					while(aux2!=NULL){
						aux3=aux2;
						aux2=aux2->brother;

					}

					aux2=(entry*)malloc(sizeof(entry));
					aux3->brother=aux2;
				}
			
				aux2->IP=token[2]; /*FILL INFO*/
				aux2->id=token[1];
				aux2->TCPport=token[3];
				aux2->fd=newfd; 
				aux2->brother=NULL;
			}

			add_neighbour(host, token[1], token[1],newfd);
			sprintf(message, "EXTERN %s %s %s\n", host->external.id, host->external.IP, host->external.TCPport);/*acaba aqui*/
			write(newfd, message, strlen(message));
			printf("[INFO]: New connection was added!\n");
			return newfd;
}

bool join (netnode *host, char *net, char *id){
	entry* data=NULL;
	printf("Net: %s\n", net);
	
	if(host->is_connected){
		printf("[FAULT]: You have already joined a network.\n Please use leave before joining a new net\n");
		return 1;
	}

	if(UDPreg(host, net, id, host->serverIP, host->serverUDP)==1){
		printf("[NET]: Cannot register to network");
		return 1;
	}

	data=UDPquery(host, net, host->serverIP, host->serverUDP);
	if(data==NULL)
		return 1;
	else
		djoin(net, id, data->id, data->IP, data->TCPport, host);
	
	return 0;
}

bool leave(netnode *host){
	char message[128]="\0";
	char servermsg[8]="\0";
	struct addrinfo hints={.ai_family = AF_INET, .ai_socktype = SOCK_STREAM};
	struct addrinfo *res;
 	struct sockaddr_in addr;
	socklen_t addrlen=sizeof(addr);
	
	if(!(host->is_connected)){
		printf("[FAULT]: Must be connected to a network to be able to withdraw one \n");
		return 1;
	}

	int errcode=getaddrinfo(host->serverIP,host->serverUDP,&hints,&res);
	if(errcode!=0)
		/*error*/ exit(1);
	sprintf(message, "UNREG %s %s\n", host->net, host->self.id);
//	printf("host: %s\n", message);
	sendto(host->UDPsocket, message, strlen(message),0,res->ai_addr,res->ai_addrlen);
	recvfrom(host->UDPsocket, servermsg, 7,0,(struct sockaddr*)&addr, &addrlen);
//	printf("Server: %s\n", servermsg);
	servermsg[7]='\0';
	if(strcmp("OKUNREG\0", servermsg)!=0){
		printf("[FAULT]: Disconnection query was not accepted\n");	
		return 1;
	}
		
	entry *aux=host->interns;
	entry *next=NULL;
	clear_routing(host);	
	while(aux!=NULL){ /*close sockets and free memory*/
		next=aux->brother;
		close(aux->fd);
		free(aux);
		aux=next;
	}
	if((host->external.fd)!=-1)
		close(host->external.fd); /*close socket*/
	if((host->TCPsocket)!=-1)
		close(host->TCPsocket);
	host->is_connected=false;
	host->external.IP=NULL; /*Reset variables to initial state*/
	host->external.fd=-1;
	host->TCPsocket=-1;
	host->interns=NULL;
	return 0;
}
