/*-----------------------------------------------------------------------------------------------------+
| iomodule.c             | Processing functions for keyboard input (stdin) and nodal (internal and     |
|                        | external) messages and actions.                                             |
|                        |                                                                             |
+------------------------------------------------------------------------------------------------------+
| Authors: Joao Barreiros C. Rodrigues nº99968, Jorge Miguel Monteiro Contente nº102143                |
|          LEEC-IST                                                                                    |
| Date: March-April 2023                                                                               |
+-----------------------------------------------------------------------------------------------------*/
#include "io.h"
#include "netstruct.h"
#include "tcp.h"
#include "content.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <ctype.h>
void proc_stdin(char* buffer, netnode *host){
	int n;
	char* buffer2 = strdup(buffer); // dup buffer to avoid writing on top of other data
	char* token[6];
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
	if (strcmp(token[0], "join") == 0){
		if(host->is_connected){
			printf("FAULT: You are already connected to a network. Please leave your network before re-connecting\n");
			return;
		}
//		printf("okay!\n");
		if(!is_number(token[1])||!is_number(token[2])){
			printf("[FAULT]:WRONG FORMAT ON COMMAND\n");
			return;
		}
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
	//	printf("okay\n");

		if(host->is_connected){
			printf("FAULT: You are already connected to a network. Please leave your network before re-connecting\n");
			return;
		}
		for(n=1; n<6;n++){
			if(token[n]==NULL || (token[n]!=NULL && (strcmp(token[n],"\0")==0))){
				printf("FAULT[001]: MISSING ARGUMENTS FOR COMMAND\n");
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
		if(is_number(token[1]))
			query_content(host, token[1], host->self.id, token[2],-1); /*Parts from the host itself so there was no previous input fd*/
										/*So we set in_fd to -1, which in a normal circumstance should never be a valid fd*/
		else
			printf("[FAULT]: WRONG FORMAT ON COMMAND\n");
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
			printf("[FAULT]: Leave failed\n");
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
    for (int i = 0; i < len; i++){
        if (candidate[i] < '0' || candidate[i] > '9')
            return false;
    }

    if (port >= 1023 && port <= 65535) /*Port 0 is reserved for stdin, max TCP/UDP port is 65535*/
        return true;
    return false;
}


void host_exit(netnode *host){
	container *aux=host->content_list, *aux2=NULL;
	if(host->is_connected){
		printf("[INFO]: Host is still connected to a network\n");
		printf("[INFO]: Will force exit. This will leave the network first.\n");
		leave(host);

	}
	while(aux!=NULL){
		aux2=aux->next;
		//free(aux->content);
		free(aux);
		aux=aux2;
	}
	free(host);
	printf("[INFO]: EXIT SUCESSFUL!\n");
	exit(0);
}

void show_topology(netnode *host){
	entry *aux=NULL;
	if(!host->is_connected){
		printf("[INFO]: You need to be connected to a network to show topology\n");
		return;
	}
	printf("--[HOST]--\n");
	printf("[%s %s %s]\n", host->self.id, host->self.IP, host->self.TCPport);
	printf("--[EXTERNAL]--\n");
	printf("[%s %s %s]\n", host->external.id, host->external.IP, host->external.TCPport);
	printf("--[BACKUP]--\n");
	printf("[%s %s %s]\n", host->backup.id, host->backup.IP, host->backup.TCPport);
	printf("--[INTERNALS]--\n");
	aux=host->interns;
	if(aux==NULL){
		printf("--empty--\n");
		return;

	}
	while(aux != NULL){
		printf("[%s %s %s]\n", aux->id, aux->IP, aux->TCPport);
		aux=aux->brother;
	}
	return;

}

void proc_extern(netnode *host){
	char *buffer=(char*)calloc(1,128*sizeof(char)), *message=(char*)calloc(1,128*sizeof(char));
	char *token[4]={NULL};
	routing_entry *aux2=host->routing_list;
	int i=0;
	entry *aux=host->interns;
	int n=read(host->external.fd,buffer,128), old_n=0;
	char *buffer2=strdup(buffer);
	bool unreacheable=false;
	/*READ INCOMPLETE PACKETS!*/
	old_n=n;
	if(n==0){
		usleep(500000);
		n=read(host->external.fd,buffer,128); /*Make sure extern has left*/
	}

	if(!(n==0 || n==-1||(strcmp(buffer,"\0")==0))){
		sleep(1);
		while(buffer[strlen(buffer)-1]!='\n'){
			if(n==old_n||n==-1||(strcmp(buffer,"\0")==0)){
				if(n==old_n)
					unreacheable=true;
				break;
			}
			else{
				old_n=n;
				n=read(host->external.fd,buffer,128);
			}
			if(n>=127)
				break;	
		}
	}

	if(unreacheable){
		printf("[FAULT]: Incomplete package received and unable to recover.\n Discarding...");
		return;
	}
	if(n==0 || n==-1||(strcmp(buffer,"\0")==0)){
	/*extern disconected*/
	remove_routing(host, host->external.id);
	sprintf(message, "WITHDRAW %s\n", host->external.id);
	while(aux2!=NULL){
		write(aux2->fd, message, strlen(message));
		aux2=aux2->next;
		usleep(2500);

	}

		close(host->external.fd);
		if(strcmp(host->self.id, host->backup.id)!=0) /*im not an anchor*/
			djoin(host->net, host->self.id, host->backup.id, host->backup.IP, host->backup.TCPport, host);
		else if(aux!= NULL){
			host->external.id=host->interns->id; /*promote first internal to anchor*/
			host->external.IP=host->interns->IP;
			host->external.TCPport=host->interns->TCPport;
			host->external.fd=host->interns->fd;
			aux=host->interns;
			host->interns=host->interns->brother; /*its no longer an intern. Repointing the head to the promoted internal brother*/
			sprintf(message, "EXTERN %s %s %s\n", host->external.id, host->external.IP, host->external.TCPport);
			add_neighbour(host, host->external.id,host->external.id,host->external.fd);
			aux=host->interns;
			while(aux!=NULL){
				add_neighbour(host, aux->id, aux->id, aux->fd);
				aux=aux->brother;
			}
			write(host->external.fd, message, strlen(message)); /*anchor is now its own backup*/		 
			usleep(2500); /*Fail-safe to prevent two messages to be sent too fast*/
		}
		else{
			host->external.id=host->self.id; /*no one to anchor, alone again*/
			host->external.IP=host->self.IP;
			host->external.TCPport=host->self.TCPport;
			host->backup.id=host->self.id;
			host->backup.IP=host->self.IP;
			host->backup.id=host->self.id;
			host->backup.TCPport=host->self.TCPport;
			host->external.fd=-1;

		}
		/*spread new backup information to interns*/
		sprintf(message, "EXTERN %s %s %s\n", host->external.id, host->external.IP, host->external.TCPport);
		aux=host->interns;
		while(aux!=NULL){
			write(aux->fd, message, strlen(message));
			aux=aux->brother;
		}	
		free(buffer);
		free(message);
		usleep(2500);
		return;
	}
	else{
		buffer=strtok(buffer,"\n");
		token[i]=strtok(buffer, " ");
		while(token[i]!=NULL){
			if (i > 3)
				break;
			i++;
			token[i]=strtok(NULL, " ");
		}
	
		if(strcmp(token[0], "EXTERN")==0){ /*Special message. Intern received instructions to replace backup*/
			host->backup.id=token[1];
			host->backup.IP=token[2];
			host->backup.TCPport=token[3];
			return;		
		}
		else if ((strcmp(token[0], "QUERY")==0)|| (strcmp(token[0], "WITHDRAW")==0)|| (strcmp(token[0], "CONTENT")==0) || (strcmp(token[0], "NOCONTENT")==0)){
			//printf("%s\n",buffer2);
			proc_contact(host, buffer2, host->external.id, host->external.fd);

		}

		else{
			printf("[FAULT]: Unknown contact from external node");
			return;
		}

	}
	return;
}

entry* proc_intern(netnode *host, entry *intern, entry *prev){
	char *buffer=(char*)calloc(1,128*sizeof(char));
	char *message=(char*)calloc(1,128*sizeof(char));
	entry *aux=host->interns;
	int n=read(intern->fd,buffer,128), old_n=0;
	
	bool first=false, unreacheable=false;
	if(n==0){
		usleep(500000);
		n=read(intern->fd,buffer,128);
	}

	if(!(n==0||n==-1||(strcmp(buffer,"\0")==0))){ /*intern hasn't yet left, but sending a incomplete package*/
			
		while(buffer[strlen(buffer)-1]!='\n'){
			sleep(1);
			if(n==old_n||n==-1||(strcmp(buffer,"\0")==0)){
				if(n==old_n)
					unreacheable=true;
				break;
			}
			else{
				old_n=n;
				n=read(intern->fd,buffer,128);
			
			}
			if(n>=127)
				break;	
		}

	
	}
	if(unreacheable){
		printf("[FAULT]: Incomplete package received and unable to recover.\n Discarding...");
		return intern;
	}
	if(n==0||n==-1||(strcmp(buffer,"\0")==0)){ /*intern left*/
	//	printf("Vou remover o %s", intern->id);
		if(intern!=host->interns)
			prev->brother=intern->brother;
			//free(intern);
		else
			first=true;

		sprintf(message, "WITHDRAW %s\n", intern->id);
		while(aux!=NULL){
			if((strcmp(aux->id, intern->id)!=0))
				write(aux->fd, message, strlen(message));
			aux=aux->brother;

		}
	

		write(host->external.fd,message,strlen(message));

		remove_routing(host, intern->id);
		close(intern->fd);
		if(first==true){
			if(host->interns->brother!=NULL)
				host->interns=host->interns->brother;
			else
				host->interns=NULL;
		}
		else
			intern=NULL;
		//free(aux); /*so many lost blocks*/
	}
	else
		proc_contact(host, buffer, intern->id, intern->fd); /*No special action. Pass to proc_contact to process*/

	free(buffer);
	free(message);
	return intern;
}


void proc_contact(netnode *host, char *buffer, char *in_id, int in_fd){
	entry *aux=host->interns;
	char *message=(char*)calloc(1,128*sizeof(char));
	int i=0;	
	char *token[4]={NULL};
	char temp[3]={'\0'};
	printf("CONTACT: %s\n",buffer);
	buffer=strtok(buffer,"\n");
	token[i]=strtok(buffer, " ");	
	while(token[i]!=NULL){
		if (i > 3)
			break;
		i++;
		token[i]=strtok(NULL, " ");
	}

	if(strcmp(token[0], "QUERY")==0){
		sprintf(temp,"%s", token[2]);
		sprintf(token[2], "%s",temp);
		if(is_number(token[2]))
			add_neighbour(host, token[2], in_id, in_fd);
		if(strcmp(host->self.id, token[1])==0)
			search_content(host, token[1], token[2], token[3]);
		else
			query_content(host, token[1], token[2], token[3], in_fd);
		return;
	}

	else if(strcmp(token[0], "WITHDRAW")==0){
		if(token[1]==NULL){
			printf("[FAULT]: WITHDRAW message with wrong format received\n");
			return;
		}
		aux=host->interns;
		sprintf(temp,"%s", token[1]); /*Fail-safe. Without temp there's a tendency to write garbage to the table*/
		sprintf(token[1], "%s",temp);
		remove_routing(host, token[1]);
		sprintf(message, "WITHDRAW %s\n", token[1]);
		printf("%s", message);
		while(aux!=NULL){
			if((aux->fd)!=in_fd)
				write(aux->fd,message,strlen(message));
			aux=aux->brother; /*Missed this line in the first submission*/
		}
		if((host->external.fd)!=in_fd)
			write(host->external.fd,message,strlen(message));
		free(message);
		return;		
	}

	else if((strcmp(token[0], "CONTENT")==0)||(strcmp(token[0], "NOCONTENT")==0)){ 
		sprintf(message, "%s %s %s %s\n", token[0], token[1], token[2], token[3]);
		if(is_number(token[2]))	
			add_neighbour(host, token[2], in_id, in_fd); /*Add node from where the message is from to the known routes*/
		if(strcmp(host->self.id, token[1])==0){
			printf("%s",message);
			fflush(stdout);
		}
		else{
			int fd=search_neighbour(host,token[1]);
			if(fd!=-1){
				if(fd!=in_fd)
					write(fd, message, strlen(message));
			}
			else
				printf("[FAULT]: ROGUE CONTENT MESSAGE DETECTED. IGNORING....\n");
			
		}
	}
	else {
		printf("[INFO]: Unknown Contact. Discarding...\n");
		return;
	}
}

bool is_number(const char* str) {
   int i = 0;

   // Check for digits
   for (; str[i] != '\0'; i++) {
      if (!isdigit(str[i]))
         return 0;
   }

   return 1;
}
