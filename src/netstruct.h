#ifndef _NETSTRUCT_H_
#define _NETSTRUCT_H_

typedef struct entry{
	struct entry* brother;
	char* id;
	char* IP;
	char* TCPport;
	char* UDPport;
	int fd;

}entry;

typedef struct netnode{
	char* net;
	int TCPsocket;
	int UDPsocket;
	struct entry self;
	struct entry backup;
	struct entry external;
	struct entry* interns;
}netnode;


#endif

