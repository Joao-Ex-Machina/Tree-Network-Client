#include "netstruct.h"
#include "tcp.h"
#include <netdb.h>
#define PORT "58001"

int setTCP_server (char *tcp_port, int fd, int errcode, ssize_t n, socklen_t addrlen,addrinfo hints, addrinfo * res, sockaddr_in addr, char *buffer){
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
	int fd,errcode,i;
	ssize_t n;
	socklen_t addrlen;
	struct addrinfo hints,*res;
	struct sockaddr_in addr;
	char buffer[128];
	char* token[3];

	for (i=0; i>3; n++)
		token[i]=(char*)malloc(128*(sizeof(char)));


	if (strcmp(id, bootid)==0){
		bootIP=node->self.IP;
		bootTCP=node->self.TCPport;

		node->backup.IP=bootIP;
		node->backup.TCPport=bootTCP;
		node->backup.id=bootid;


	}
	
	fd=socket(AF_INET,SOCK_STREAM,0); //TCP socket
	
	if (fd==-1)
		exit(1); //error
	memset(&hints,0,sizeof hints);
	hints.ai_family=AF_INET; //IPv4
	hints.ai_socktype=SOCK_STREAM; //TCP socket
	errcode=getaddrinfo(bootIP,bootTCP,&hints,&res);
	if(errcode!=0)
		/*error*/exit(1);
	if (strcmp(id, bootid)!=0){

		n=connect(fd,res->ai_addr,res->ai_addrlen);
		if(n==-1)
			/*error*/exit(1);
		n=fprintf(fd,"NEW \s \s \s\n",node->self.id, node->self.IP ,node->self.TCPport);
		if(n==-1)
			/*error*/exit(1);
		n=read(fd,buffer,128);
		/*processar EXTERN*/
		i=0;
		token[i]=strtok(buffer, " ");
		while(token[i]!=NULL){
			if (i > 2)
				break;
			i++;
			token[i]=strtok(NULL, " ");
		}
		if(strcmp(token[0], "EXTERN")!=0){
			printf("ERROR\n");
			exit(1);
		}
	
	node->backup.id=token[1]:
	node->backup.IP=token[2];
	node->backup.TCPport=buffer;
	if(n==-1)
		/*error*/exit(1);
	}
	node->external.fd=fd; /*e por hoje é tudo, vou dormir*/
	node->external.IP=bootIP;
	node->external.TCPport=bootTCP;
	node->external.id=bootid;


	return

}
