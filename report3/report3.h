#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>

#define BUF_SIZE		100
#define ARG_SIZE		50

//args.c
int analysys_args(char**, char*);
int search_pipe(int, char**);
int search_redirect(int, char**);

//myexec.c
int myexec(int, char**);
