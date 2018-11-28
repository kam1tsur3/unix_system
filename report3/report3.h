#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#define BUF_SIZE		100
#define ARG_SIZE		50

//args.c
int analysys_args(char**, char*);
int search_token(int, char**);

//process.c
//void newpro(int, char**);
//void search_exec(int ,char **);
