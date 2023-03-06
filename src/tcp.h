#ifndef _TCP_H_
#define _TCP_H_
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
typedef struct sockaddr_in sockaddr_in;
typedef struct addrinfo addrinfo;
void setTCP_server(char* tcp_port,int fd,int errcode,ssize_t n, socklen_t addrlen,addrinfo hints,addrinfo *res, sockaddr_in addr,char* buffer);
void djoin (char* net, char* id, char* bootid, char* bootIP, char* bootTCP);
#endif

