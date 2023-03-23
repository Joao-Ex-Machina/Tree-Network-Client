#include "netstruct.h"
#include "io.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

void create_content(netnode *host, char* message){
	struct container *aux=host->content_list, *aux2=host->content_list;
	while(aux != NULL){
		aux2=aux;
		aux=aux->next;
	}
	aux=(container*)malloc(sizeof(container));
	if(aux2 != aux){
		if(aux2!=NULL)
			aux2->next=aux;
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
	printf("--[START OF CONTENT LIST]--");
	while(aux!=NULL){
		printf("%s\n", aux->content);
		aux=aux->next;
	}
	printf("--[END OF CONTENT LIST]--");
	return;
}

void query_content(netnode *host, char *dest, char *origin, char * query){
				

}
