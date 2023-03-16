#ifndef _NETSTRUCT_H_
#define _NETSTRUCT_H_
#include <stdbool.h>
typedef struct entry{
	struct entry* brother;
	char* id;
	char* IP;
	char* TCPport;
	char* UDPport;
	int fd;

}entry;

typedef struct netnode{
	bool is_connected;
	char* net;
	int TCPsocket;
	int UDPsocket;
	char* serverIP;
	char* serverUDP;
	struct entry self;
	struct entry backup;
	struct entry external;
	struct entry* interns;
}netnode;


#endif

