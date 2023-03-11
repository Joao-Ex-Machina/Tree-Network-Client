#include "netstruct.h"
#include "tcp.h"
#include "udp.h"

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
	struct addrinfo hints,*res;
	char buffer[128];
	char* token[3];
	printf("%s %s\n",id,bootid);
	printf("ENTREI NO DJOIN\n");
	
	for (i=0; i<3; i++)
		token[i]=(char*)malloc(128*(sizeof(char)));
	printf("acabei setup\n");
	printf("é isto: %d \n", strcmp(id, bootid));
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
		printf("A sair do djoin\n");
		return;
	}

	fd=socket(AF_INET,SOCK_STREAM,0); //TCP socket
	printf("%d", fd);
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
		n=fprintf(fdopen(fd, "w"),"NEW %s %s %s\n",node->self.id, node->self.IP ,node->self.TCPport);
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
	
	node->backup.id=token[1];
	node->backup.IP=token[2];
	node->backup.TCPport=buffer;
	if(n==-1)
		/*error*/exit(1);
	}

	node->external.fd=fd; /*e por hoje é tudo, vou dormir*/
	node->external.IP=bootIP;
	node->external.TCPport=bootTCP;
	node->external.id=bootid;
	printf("A sair do djoin\n");

	return;

}

int handshake(netnode *host,addrinfo hints, addrinfo *res, sockaddr_in addr, char *buffer, fd_set rfds){
	char* token[3]; /*função*/
	int newfd,i;
	netnode* aux=host;
			printf("ENTER HANDSHAKE\n");
			for (i=0; i<3; i++)
				token[i]=(char*)malloc(128*(sizeof(char)));
			FD_CLR(host->TCPsocket, &rfds);
			socklen_t addrlen = sizeof (addr);
			if ((newfd = accept (host->TCPsocket, &addr, &addrlen)) == -1)
			  return newfd;

			fgets(buffer,128,fdopen(newfd, "r"));
			i=0;

			token[i]=strtok(buffer, " ");
			while(token[i]!=NULL){
				if (i > 2)
					break;
				i++;
				token[i]=strtok(NULL, " ");
			}

			if(strcmp(token[0], "NEW")!=0){
				printf("WRONG FORMAT ON OUTSIDE CLIENT\n");
				for (int i=0; i<3; i++){
					free(token[i]);
					return -1;
				
				}
			}

			if(aux->interns==NULL){
				aux->interns=(entry*)malloc(sizeof(entry));
			}
			else{
				while(aux->interns->brother!=NULL){
					aux->interns=aux->interns->brother;

				}

				aux->interns->brother=(entry*)malloc(sizeof(entry));
				aux->interns=aux->interns->brother;
			}
			strcpy(buffer, strtok(buffer, "\n"));
			aux->interns->IP=token[1];
			aux->interns->id=token[2];
			aux->interns->TCPport=buffer;
			aux->interns->fd=newfd; /*Passar por referência*/
			aux=host;
			/* Caso host->backup.id == host->self.id*/
			/*Copiar estes aux->interns todos para o backup*/
			if(strcmp(host->backup.id, host->self.id)){
				host->backup.IP=token[1];
				aux->backup.id=token[2];
				aux->backup.TCPport=buffer;
				aux->backup.fd=newfd;
			}

			fprintf(fdopen(newfd, "w"), "EXTERN %s %s %s\n", host->external.id, host->external.IP, host->external.TCPport);/*acaba aqui*/
			printf("Adeus!\n");
			return newfd;
}

bool join (netnode *host, char *net, char *id){
	entry* data=NULL;
	if(atoi(id)<100){
		id[2]=id[1];
		id[1]=id[0];
		id[0]='0';
		id[3]='\0';
	}
	if(UDPreg(host, net, id)==0){
		return 0;
	}
	data=UDPquery(host, net);
	if(data==NULL)
		djoin(net, id, id, NULL, NULL, host);
	else
		djoin(net, id, data->id, data->IP, data->TCPport, host);
	
	return 1;
}

