#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

#define BUF_SIZE		100
#define ARG_SIZE		50

//args.c
void analysys_args(char**, char*);
int search_token(int, char**);

//process.c
void newpro(int, char**);
