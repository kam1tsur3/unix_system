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
#define BG_SIZE         20
#define FG_SIZE         20

int bg_counter;

int fg_buf[FG_SIZE];
int fg_head;
int fg_tail;

//args.c
int analysys_args(char**, char*);
int search_pipe(int, char**);
int search_redirect(int, char**);
void rebuf(char *, char *);

//myexec.c
int myexec(int, char**);
