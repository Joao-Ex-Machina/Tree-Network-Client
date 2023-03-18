#ifndef _IO_H_
#define _IO_H_
#include "netstruct.h"
#include "tcp.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
void proc_stdin(char* buffer, netnode *host);
bool Is_ValidIPv4(const char *candidate);
bool Is_ValidPort(const char *candidate);
void show_topology(netnode *host);
void host_exit(netnode *host);
#endif

