#ifndef _NETSTRUCT_H_
#define _NETSTRUCT_H_
typedef struct node{
	char* net;
	struct entry self;
	struct entry backup;
	struct entry external;
	struct entry* interns;
}node;

typedef struct entry{
	struct entry* brother;
	char* id;
	char* IP;
	char* TCPport;

}entry;

#endif

