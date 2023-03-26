#include "netstruct.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
void free_entry(entry *candidate){
	free(candidate->IP);
	free(candidate->TCPport);
	free(candidate->UDPport);
	free(candidate->id);
	close(candidate->fd);
	free(candidate);
}
void free_host(netnode *host){
	entry *aux=host->interns, *aux2=NULL;
	while(aux!=NULL){
		aux2=aux->brother;
		free_entry(aux);
		aux=aux2;
	}
	free(host->serverIP); /*Will continue another time*/
}
