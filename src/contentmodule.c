#include "content.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void create_content(netnode *host, char* message){
	struct container *aux=host->content_list, *aux2=host->content_list;
	printf("Vou criar um conteúdo chamado %s", message);
	if(aux==NULL){
		host->content_list=malloc(sizeof(container));
		aux=host->content_list;
	}
	else{
		while(aux != NULL){
			aux2=aux;
			aux=aux->next;
		}

		aux=(container*)malloc(sizeof(container));
		if(aux2 != aux){
			if(aux2!=NULL)
				aux2->next=aux;
		}
	}
	aux->content=message;
	return;
}

void remove_content(netnode *host, char* candidate){
	struct container *aux=host->content_list,*aux2=host->content_list;
	while(aux!=NULL){
		if(strcmp(aux->content, candidate)==0){
			if(aux != aux2)
				aux2->next=aux->next;
			else
				host->content_list=aux->next;
			free(aux);
			printf("[INFO]: Deleted content %s\n", candidate);
			return;
		}
		aux2=aux;
		aux=aux->next;
	}
	printf("[INFO]: No content named %s cached\n", candidate);
	return;
}

void show_names(netnode *host){
	struct container *aux=host->content_list;
	if(aux==NULL){
		printf("[INFO]: No contents\n");
		return;
	}
	printf("--[START OF CONTENT LIST]--\n");
	while(aux!=NULL){
		printf("%s\n", aux->content);
		aux=aux->next;
	}
	printf("--[END OF CONTENT LIST]--\n");
	return;
}

void query_content(netnode *host, char *dest, char *origin, char * query){
	char *message=(char*)malloc(128*(sizeof(char)));
	entry *aux=host->interns;
	if(host->is_connected == false){
		printf("[INFO]: Cannot query while outside a network");
		return;
	}
	sprintf(message, "QUERY %s %s %s\n", dest, origin, query);/*acaba aqui*/
	while (aux != NULL){
		write(aux->fd, message, strlen(message));
		aux=aux->brother;
	}
	free(message);
	return;
}
void search_content(netnode *host, char *dest, char *origin, char *query){
	container *aux=host->content_list;
	bool query_exists=false;
	char *message=(char*)malloc(128*sizeof(char));
	while(aux!=NULL){
		if(strcmp(query, aux->content)==0){
			query_exists=true;
			break;
		
		}
		aux=aux->next;
	}

	if(query_exists){
		sprintf(message, "CONTENT %s %s %s\n", origin, dest, query);
	}

	else{
		sprintf(message, "NOCONTENT %s %s %s\n", origin, dest, query);
	}
	write(host->external.fd, message, strlen(message));
	free(message);
	return;
}