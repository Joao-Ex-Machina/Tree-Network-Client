#include "tcp.h"
#include <netdb.h>
#define PORT "58001"

void setTCP_server (char *tcp_port, int fd, int errcode, ssize_t n, socklen_t addrlen,addrinfo hints, addrinfo * res, sockaddr_in addr, char *buffer){
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
		exit (1);			/*error */
}

void djoin (char* net, char* id, char* bootid, char* bootIP, char* bootTCP){
	int fd,errcode;
	ssize_t n;
	socklen_t addrlen;
	struct addrinfo hints,*res;
	struct sockaddr_in addr;
	char buffer[128];
	char* hostIP;
	int hostname;
	struct hostent *host_entry;

	if (strcmp(id, bootid)==0){
		hostname = gethostname(buffer, sizeof(buffer));
		if(hostname == -1)
			exit(1);
		host_entry = gethostbyname(buffer);
		if(host_entry==NULL)
			exit(1);
		hostIP = inet_ntoa(*((struct in_addr*)host_entry->h_addr_list[0]));
		bootIP=hostIP;

	}
	fd=socket(AF_INET,SOCK_STREAM,0); //TCP socket
	if (fd==-1)
		exit(1); //error
	memset(&hints,0,sizeof hints);
	hints.ai_family=AF_INET; //IPv4
	hints.ai_socktype=SOCK_STREAM; //TCP socket
	errcode=getaddrinfo(bootIP,PORT,&hints,&res);
	if(errcode!=0)
		/*error*/exit(1);
	n=connect(fd,res->ai_addr,res->ai_addrlen);
	if(n==-1)
		/*error*/exit(1);
	n=write(fd,"Hello!\n",7);
	if(n==-1)
		/*error*/exit(1);
	n=read(fd,buffer,128);
	if(n==-1)
		/*error*/exit(1);
	write(1,"echo: ",6); write(1,buffer,n);
}
