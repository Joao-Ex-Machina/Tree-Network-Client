#include "io.h"
#include "netstruct.h"
#include "tcp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void proc_stdin(char* buffer, netnode *host){
	int n;
	char* token[6];
//	for (n=0; n<6; n++)
//		token[n]=(char*)calloc(1,128*(sizeof(char)));
	n=0;
	buffer=strtok(buffer,"\n");
	token[n]=strtok(buffer, " ");
	while(token[n]!=NULL){
		if (n > 5)
			break;
		n++;

		token[n]=strtok(NULL, " ");
			//free(aux);
	}
	n=0;
	while(n<6){
		if(token[n]!= NULL){

			printf(token[n]);
			printf("\n");
			n++;
		
		}
		else{
			break;
		
		}
	}

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
	char in;
	if(host->is_connected){
		printf("[INFO]: Host is still connected to a network\n");
		printf("[INFO]:Want to force exit? (y/N) ");
		scanf("%c",&in);
		if(in=='y'|| in=='Y'){
			leave(host);
			free(host);
			exit(0);
		}

	}
	else{
		free(host);
		exit(0);
	}
}
