#include "main.h"
#include "tcp.h"

int main (int argc, char *argv[]){
  ssize_t n;
  socklen_t addrlen;
  struct addrinfo hints, *res;
  struct sockaddr_in addr;
  char buffer[128];
  int fd, errcode, newfd, afd = 0;
  fd_set rfds;
  enum{ idle, busy } state;
  int maxfd, counter;
//      int regIP = [193,136,138,142];
  char *regUDP = "59000";
  char *tcp_port = "58001";
/*	if( argc < 3 || argc > 5){
        	printf("Por favor verifique se usou todos os dados necessários e tente outra vez.\n");
        	exit(EXIT_FAILURE);
    	}

    	int main_IP = inet_aton(argv[1]);
    	tcp_port = strtol(argv[2], NULL, 10);

    	if (argc >= 4){
        	regIP =  inet_aton(argv[3]);
		if (argc == 5)
        		regUDP =  strtol(argv[4], NULL, 10);
    	}
*/
setTCP_server (tcp_port, fd, errcode, n, addrlen, hints, res, addr, buffer);

  while (1){
      FD_ZERO (&rfds);
      switch (state){
	case idle:
	  FD_SET (fd, &rfds);
	  maxfd = fd;
	  break;
	case busy:
	  FD_SET (fd, &rfds);
	  FD_SET (afd, &rfds);
	  maxfd = max (fd, afd);
	  break;
	}			//switch(state)
      counter =select (maxfd + 1, &rfds, (fd_set *) NULL, (fd_set *) NULL,(struct timeval *) NULL);
      if (counter <= 0)
	/*error */ exit (1);
      for (; counter; --counter)
	switch (state){
	  case idle:
	    if (FD_ISSET (fd, &rfds)){
		FD_CLR (fd, &rfds);
		addrlen = sizeof (addr);
		if ((newfd = accept (fd, &addr, &addrlen)) == -1)
		  /*error */ exit (1);
		afd = newfd;
		state = busy;
	      }
	    break;
	  case busy:
	    if (FD_ISSET (fd, &rfds)){
		FD_CLR (fd, &rfds);
		addrlen = sizeof (addr);
		if ((newfd = accept (fd, &addr, &addrlen)) == -1)
		  /*error */ exit (1);
		close (newfd);
	      }
	    else if (FD_ISSET (afd, &rfds)){
		FD_CLR (afd, &rfds);
		if ((n = read (afd, buffer, 128)) != 0){
		    if (n == -1)
		      /*error */ exit (1);
		    /* ... write buffer in afd */
		  }
		else{
		    close (afd);
		    state = idle;
		}		//connection closed by peer
	      }
	    break;
	  }			//switch(state)
  	}				//while(1)
   	freeaddrinfo(res);
   	close(fd);
	return 0;
}
