#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/stat.h>

#define	DATASIZE	1024
#define HEADERSIZE	4
#define PACKSIZE	1028
#define MYPORT		50021
#define ARG_SIZE	50
#define BUF_SIZE	100		
#define TMPFILE "tmpfile"
struct myftph {
	uint8_t type;
	uint8_t code;
	uint16_t length;
};

struct myftph_data{
	uint8_t type;
	uint8_t code;
	uint16_t length;
	char data[DATASIZE];
};

//args.c
int analysys_args(char **, char *);

//err.c
int err_check(char, char);
