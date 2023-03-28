#include "io.h"
#include "netstruct.h"
#include "tcp.h"
#include "content.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


void proc_stdin(char* buffer, netnode *host){
	int n;
	char* buffer2 = strdup(buffer); // dup buffer to avoid writing on top of other data
	char* token[6];
//	for (n=0; n<6; n++)
//		token[n]=(char*)calloc(1,128*(sizeof(char)));
	n=0;
	buffer=strtok(buffer2,"\n");
	token[n]=strtok(buffer2, " ");
	while(token[n]!=NULL){
		if (n > 5)
			break;
		n++;

		token[n]=strtok(NULL, " ");
			//free(aux);
	}
	n=0;
/*	while(n<6){
		if(token[n]!= NULL){

			printf(token[n]);
			printf("\n");
			n++;
		
		}
		else{
			break;
		
		}
	}*/

	if (strcmp(token[0], "join") == 0){
		if(host->is_connected){
			printf("FAULT: You are already connected to a network. Please leave your network before re-connecting\n");
			return;
		}
		printf("okay!\n");
		for(n=1; n<3;n++){
			if(token[n]==NULL || (token[n]!=NULL && strcmp(token[n],"\0"))==0){
				printf("FAULT[001]: MISSING ARGUMENTS FOR COMMAND\n");
				return;
			}
		}
		join(host, token[1], token[2]);
		return;
		
	}	
	else if (strcmp(token[0], "djoin") == 0){
		printf("okay\n");

		if(host->is_connected){
			printf("FAULT: You are already connected to a network. Please leave your network before re-connecting\n");
			return;
		}
		for(n=1; n<6;n++){
			if(token[n]==NULL || (token[n]!=NULL && strcmp(token[n],"\0"))==0){
				printf("FAULT[001]: MISSING ARGUMENTS FOR COMMAND\n");
				/*for (n=0; n<6; n++){
					if(token[n]!=NULL)
						free(token[n]);
				}*/

				return;
			
			

			}
		}
		djoin(token[1], token[2], token[3], token[4], token[5], host);
		return;
	}
	else if((strcmp(token[0], "st")==0) || ((strcmp(token[0], "show")==0)&&(strcmp(token[1], "topology")==0))){
		show_topology(host);
		free(buffer2);
		return;
	}
	else if(strcmp(token[0], "create")==0){
		create_content(host, token[1]);
		return;

	}
	else if(strcmp(token[0], "delete")==0){
		remove_content(host, token[1]);
		return;
	}
	else if (strcmp(token[0], "get")==0) {
		query_content(host, token[1], host->self.id, token[2]);
		return;
	
	}
	else if((strcmp(token[0],"sn")==0)|| ((strcmp(token[0],"show")==0)&&(strcmp(token[1],"names")==0))){
		show_names(host);
		return;
	}
	else if((strcmp(token[0],"cr")==0)|| ((strcmp(token[0],"clear")==0)&&(strcmp(token[1],"routing")==0))){
		clear_routing(host);
		return;
	}
	else if((strcmp(token[0],"sr")==0)|| ((strcmp(token[0],"show")==0)&&(strcmp(token[1],"routing")==0))){
		show_routing(host);
		return;
	}
	else if(strcmp(token[0], "leave")==0){
		if(leave(host)){
			printf("[FAULT]: Leave failed");
		}
		free(buffer2);
		return;
	}
	else if(strcmp(token[0], "exit")==0){
		free(buffer2);
		host_exit(host);
		return;
	
	}
	else{
		printf("[Fault][000]: Unknown Command \n");
	}
	for (n=0; n>6; n++)
		//free(token[n]);

	return;
}

bool Is_ValidIPv4(const char *candidate){
    in_addr_t ip;
    if (inet_pton(AF_INET, candidate, &ip) == 1) /*Converting text to binary*/
        return true;
    return false;
}

bool Is_ValidPort(const char *candidate){
    int port = atoi(candidate);
    int len = strlen(candidate);
    for (int i = 0; i < len; i++)
    {
        if (candidate[i] < '0' || candidate[i] > '9')
            return false;
    }

    if (port >= 0 && port <= 65535) /*Port 0 is reserved for stdin, max TCP/UDP port is 65535*/
        return true;
    return false;
}


void host_exit(netnode *host){
	if(host->is_connected){
		printf("[INFO]: Host is still connected to a network\n");
		printf("[INFO]: Will force exit. This will leave the network first.");
		leave(host);	
		free(host);
		exit(0);

	}
	else{
		free(host);
		exit(0);
	}
}
void show_topology(netnode *host){
	entry *aux=NULL;
	if(!host->is_connected){
		printf("[INFO]: You need to be connected to a network to show topology\n");
		return;
	}
	printf("EXTERNAL\n");
	printf("%s %s %s\n", host->external.id, host->external.IP, host->external.TCPport);
	printf("BACKUP\n");
	printf("%s %s %s\n", host->backup.id, host->backup.IP, host->backup.TCPport);
	printf("INTERNALS\n");
	aux=host->interns;
	if(aux==NULL){
		printf("--empty--\n");
		return;

	}
	while(aux != NULL){
		printf("%s %s %s\n", aux->id, aux->IP, aux->TCPport);
		aux=aux->brother;
	}
	return;

}

void proc_extern(netnode *host){
	char *buffer=(char*)malloc(128*sizeof(char)), *message=(char*)malloc(128*sizeof(char));
	char *token[4];
	int i=0;
	entry *aux=host->interns;
	int n=read(host->external.fd,buffer,128);
	char *buffer2=strdup(buffer);
	if(n==0 || n==-1){
		/*extern disconected*/
		close(host->external.fd);
		if(strcmp(host->self.id, host->backup.id)!=0) /*im not an anchor*/
			djoin(host->net, host->self.id, host->backup.id, host->backup.IP, host->backup.TCPport, host);
		else if(aux!= NULL){
			host->external.id=host->interns->id; /*promote first internal to anchor*/
			host->external.IP=host->interns->IP;
			host->external.TCPport=host->interns->TCPport;
			host->external.fd=host->interns->fd;
			aux=host->interns;
			host->interns=host->interns->brother;
			free(aux); /*better free here*/ /*its no longer an intern*/
		}
		else{
			host->external.id=host->self.id; /*no one to anchor, alone again*/
			host->external.IP=host->self.IP;
			host->external.TCPport=host->self.TCPport;
			host->external.fd=-1;

		}
		/*spread information*/
		sprintf(message, "EXTERN %s %s %s\n", host->external.id, host->external.IP, host->external.TCPport);
		aux=host->interns;
		while(aux!=NULL){
			write(aux->fd, message, strlen(message));
			aux=aux->brother;
		}	
		free(buffer);
		free(message);

		return;
	}
	else{
		token[i]=strtok(buffer, " ");
		while(token[i]!=NULL){
			if (i > 3)
				break;
			i++;
			token[i]=strtok(NULL, " ");
		}
	
		if(strcmp(token[0], "EXTERN")==0){
			host->backup.id=token[1];
			host->backup.IP=token[2];
			host->backup.TCPport=token[3];
			return;		
		}
		else if ((strcmp(token[0], "QUERY")==0)|| (strcmp(token[0], "WITHDRAW")==0)|| (strcmp(token[0], "CONTENT")==0) || (strcmp(token[0], "NOCONTENT")==0)){
			printf("%s\n",buffer2);
			proc_contact(host, buffer2, host->external.id, host->external.fd);

		}

		else{
			printf("[FAULT]: Unknown contact from external node");
			return;
		}

	}
	return;
}

void proc_intern(netnode *host, entry *intern, entry *prev){
	char *buffer=(char*)malloc(128*sizeof(char));
	entry *aux=NULL;
	int n=read(intern->fd,buffer,128);
	if(n==0){ /*intern left*/
		if(intern!=host->interns)
			prev->brother=intern->brother;
		else{
			close(intern->fd);
			free(intern);
			host->interns=NULL;
			return;

		}
		close(intern->fd);
		aux=intern;
		intern=NULL;
		free(aux); /*so many lost blocks*/
	}
	else
		proc_contact(host, buffer, intern->id, intern->fd);
	return;
}


void proc_contact(netnode *host, char *buffer, char *in_id, int in_fd){
	entry *aux=host->interns;
	char *message=(char*)malloc(128*sizeof(char));
	int i=0;	
	char *token[4];
	buffer=strtok(buffer, "\n");
	printf("%s",buffer);
	token[i]=strtok(buffer, " ");
	
	while(token[i]!=NULL){
		if (i > 3)
			break;
		i++;
		token[i]=strtok(NULL, " ");
	}

	if(strcmp(token[0], "QUERY")==0){
		if(strcmp(host->self.id, token[1])==0)
			search_content(host, token[1], token[2], token[3]);
		else
			query_content(host, token[1], token[2], token[3]);
		add_neighbour(host, token[2], in_id, in_fd);
		return;
	}

	else if(strcmp(token[0], "WITHDRAW")==0){
		aux=host->interns;
		remove_routing(host, token[1]);
		sprintf(message, "WITHDRAW %s\n", token[1]);
		while(aux!=NULL){
			if((aux->fd)!=in_fd)
				write(aux->fd,message,sizeof(message));
		}
		if((host->external.fd)!=in_fd)
			write(aux->fd,message,sizeof(message));
		free(message);
		return;		
	}

	if((strcmp(token[0], "CONTENT")==0)||(strcmp(token[0], "NOCONTENT")==0)){ 
		sprintf(message, "%s %s %s %s\n", token[0], token[1], token[2], token[3]);
		if(strcmp(host->self.id, token[1])==0)
			printf("%s",message);
		else
			write(host->external.fd, message, strlen(message));
		add_neighbour(host, token[2], in_id, in_fd);

	}
}
