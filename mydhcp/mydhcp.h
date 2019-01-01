#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>

#define	PAIR_MAX	50
#define CLIENT_MAX	50
#define MYPORT		51230
#define TIMEOUT		5	
struct ip_pair{
	struct ip_pair *fp;
	struct ip_pair *bp;
	char ip_addr[20];
	char netmask[20];
};

struct client{
	struct client *fp;
	struct client *bp;
	short status;
	int ttlcounter;

	struct in_addr id;
	struct in_addr addr;
	struct in_addr netmask;
	struct ip_pair *given;
	uint16_t ttl;
};

//list.c
void addpair_tail(struct ip_pair*, struct ip_pair*);
void rmpair(struct ip_pair*);
void addclient_tail(struct client*, struct client*);
void rmclient(struct client*);
struct client *search_client(struct client *, in_addr_t);
