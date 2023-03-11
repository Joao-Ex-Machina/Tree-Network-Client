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

	if (strcmp(token[0], "join") == 0)
		printf("okay\n");
	else if (strcmp(token[0], "djoin") == 0){
		printf("okay\n");
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
