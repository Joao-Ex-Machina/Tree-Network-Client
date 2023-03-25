#ifndef _CONTENT_H_
#define _CONTENT_H_
#include "netstruct.h"
#include "io.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
void create_content(netnode *host, char* message);
void remove_content(netnode *host, char* candidate);
void show_names(netnode *host);
void query_content(netnode *host, char *dest, char *origin, char * query);
void search_content(netnode *host, char *dest, char *origin, char *query);
#endif

