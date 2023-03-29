#include "content.h"
#include "netstruct.h"
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
	routing_entry *aux=host->routing_list;
	if(host->is_connected == false){
		printf("[INFO]: Cannot query while outside a network");
		return;
	}
	int fd=search_neighbour(host, dest);
	sprintf(message, "QUERY %s %s %s\n", dest, origin, query);/*acaba aqui*/
	if(fd==-1){
		while (aux != NULL){
			write(aux->fd, message, strlen(message));
			aux=aux->next;
		}
	}
	else
		write (fd, message, strlen(message));
	free(message);
	return;
}

void search_content(netnode *host, char *dest, char *origin, char *query){
	struct container *aux=host->content_list;
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
	int fd=search_neighbour(host, origin);
	if(fd==-1)
		printf("[INFO]: I have not queried this CONTENT");
	else
		write(fd, message, strlen(message));
	free(message);
	return;
}

void add_neighbour(netnode *host, char *dest, char *neighbour, int fd){
	struct routing_entry *aux=host->routing_list, *aux2=host->routing_list;
	printf("Vamos começar a adicionar routes\n");
	if (aux==NULL){
		printf("Adicionar o primeiro bro\n");
		host->routing_list=(routing_entry *)calloc(1,sizeof(routing_entry));
		aux=host->routing_list;
		aux->dest=dest;
		aux->neighbour=neighbour;
		aux->fd=fd;
		return;

	}
	while (aux!=NULL){
		if(strcmp(aux->dest, dest)==0 && strcmp(aux->neighbour, neighbour)==0)
			return; /*there is already a entry with this info*/
		aux2=aux;
		aux=aux->next;
	}
	
	aux=(routing_entry *)calloc(1,sizeof(routing_entry));
	aux->dest=dest;
	aux->neighbour=neighbour;
	aux->fd=fd;
	aux2->next=aux;
	return;

}

void remove_routing(netnode *host, char *candidate){
	struct routing_entry *aux=host->routing_list, *aux2=host->routing_list;
	while(aux!=NULL){
		if((strcmp(aux->dest, candidate)==0)||(strcmp(aux->neighbour, candidate)==0)){
			aux2->next=aux->next;
			aux=NULL;
			//free(aux);
		}

		aux2=aux;
		if(aux!=NULL)
			aux=aux->next;
	}
	return;
}

void clear_routing(netnode *host){
	entry *aux3=host->interns;
	if(!host->is_connected){
		printf("Must be connect to a network to show routing");
		return;
	}
	struct routing_entry *aux=host->routing_list, *aux2=host->routing_list;
	while(aux!=NULL){
		aux2=aux->next;
		aux=NULL;
		//free(aux);
		aux=aux2;
	}
	printf("[INFO]: Routing cleared! Reloading external and internal routes\n");
	add_neighbour(host, host->external.id, host->external.id, host->external.fd);
	while(aux3!=NULL){
		add_neighbour(host, aux3->id, aux3->id, aux3->fd);
		aux3=aux3->brother;
	}
	printf("[INFO]: Base routes reloaded.\n");

}

void show_routing(netnode *host){
	struct routing_entry *aux=host->routing_list;
	if(aux==NULL){
		printf("[INFO]: No routing entries\n");
		return;
	}
	printf("--[START OF ROUTING LIST]--\n");
	printf("---[Destiny | Neighbour]---\n");
	while(aux!=NULL){
		printf("[%s | %s]->%d\n", aux->dest, aux->neighbour, aux->fd);
		aux=aux->next;
	}
	printf("--[END OF ROUTING LIST]--\n");
	return;
}

int search_neighbour(netnode *host, char *dest){
	routing_entry *aux=host->routing_list;
	int fd=-1;
	while(aux!=NULL){
		if(strcmp(dest, aux->dest)==0){
			fd=aux->fd;
			break;
		}
		aux=aux->next;

	}
	return fd;
}
