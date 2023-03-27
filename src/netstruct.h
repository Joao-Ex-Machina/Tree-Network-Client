#ifndef _NETSTRUCT_H_
#define _NETSTRUCT_H_
#include <stdbool.h>
#include <netdb.h>

typedef struct entry{
	struct entry *brother;
	char *id;
	char *IP;
	char *TCPport;
	char *UDPport;
	int fd;

}entry;

typedef struct container{
	char *content;
	struct container *next;
}container;

typedef struct routing_entry{
	struct routing_entry *next;
	char *dest;
	char *neighbour;
	int fd;
}routing_entry;

typedef struct netnode{
	fd_set rfds;
	bool is_connected;
	char *net;
	int TCPsocket;
	int UDPsocket;
	char *serverIP;
	char *serverUDP;
	struct entry self;
	struct entry backup;
	struct entry external;
	struct entry* interns;
	struct container *content_list; 
	struct container *query_list;
	struct routing_entry *routing_list;
}netnode;


#endif

