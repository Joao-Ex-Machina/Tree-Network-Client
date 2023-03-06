#compiler
CC = gcc
#compile flags (in the valgrind debugging stage the flag -g was also being used)
CFLAGS  = -Wall -std=gnu99 -g
#executable
default: cot
#Linking
cot:  cotmain.o tcpmodule.o iomodule.o
	$(CC) $(CFLAGS) -o cot cotmain.o tcpmodule.o iomodule.o
#Compilation of the object files
cotmain.o:  
	$(CC) $(CFLAGS) -c cotmain.c

tcpmodule.o: 
	$(CC) $(CFLAGS) -c tcpmodule.c

iomodule.o: 
	$(CC) $(CFLAGS) -c iomodule.c

#remove object files and emac backup files from directory before forcing recompilation
clean:
	$(RM) count *.o *~ && $(MAKE)

#remove object files and emac backup files from directory before forcing recompilation
sweep:
	$(RM) count *.o *~
