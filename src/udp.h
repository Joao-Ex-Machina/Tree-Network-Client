#ifndef _UDP_H_
#define _UDP_H_
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include "netstruct.h"
#include "tcp.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

int UDPconnect(char* regIP, char* regUDP);
entry* UDPquery (netnode *host, char *net);
bool UDPreg(netnode *host, char *net, char *id);
#endif

