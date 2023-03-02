#include "tcp.h"
#include <netdb.h>
void setTCP_server (char *tcp_port, int fd, int errcode, ssize_t n, socklen_t addrlen,addrinfo hints, addrinfo * res, sockaddr_in addr, char *buffer){
	fd = socket (AF_INET, SOCK_STREAM, 0);	//TCP socket
	if (fd == -1)
		exit(1);			/*error */
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;	//IPv4
	hints.ai_socktype = SOCK_STREAM;	//TCP socket
	hints.ai_flags = AI_PASSIVE;
	errcode = getaddrinfo (NULL, tcp_port, &hints, &res);
	if((errcode) != 0)
		exit (1);			/*error */
	n = bind (fd, res->ai_addr, res->ai_addrlen);
	if(n == -1)
		exit (1);			/*error */
	if(listen (fd, 5) == -1)
		exit (1);			/*error */
}

