#include "io.h"
#include "tcp.h"
#include <stdio.h>
#include <stdlib.h>


void proc_stdin(char* buffer){
	int n;
	char* token[6];
	for (n=0; n>6; n++)
		token[n]=(char*)malloc(128*(sizeof(char)));
	n=0;
	token[n]=strtok(buffer, " ");
	while(token[n]!=NULL){
		if (n > 5)
			break;
		n++;
		token[n]=strtok(NULL, " ");
	}
	n=0;
/*	while(n<=5){
		if(token[n]!= NULL){

			printf(token[n]);
			printf("\n");
			n++;
		
		}
		else{
			break;
		
		}
	}
*/
	if (strcmp(token[0], "join") == 0)
		printf("okay\n");
	else if (strcmp(token[0], "djoin") == 0){
		printf("okay\n");
		djoin(token[1], token[2], token[3], token[4], token[5]);
	}

	else{
		printf("[Fault][000]: Unknown Command \n");
	}
	return;
}
