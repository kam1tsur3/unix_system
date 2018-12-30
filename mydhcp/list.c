#include "mydhcp.h"

void addpair_tail(struct ip_pair *head, struct ip_pair *new)
{
	new->fp = head;
	new->bp = head->bp;
	head->bp->fp = new;
	head->bp = new;
}

void rmpair(struct ip_pair *pair)
{
	pair->fp->bp = pair->bp;
	pair->bp->fp = pair->fp;
	pair->fp = pair->bp = NULL;
}

void addclient_tail(struct client *head, struct client *new)
{
	new->fp = head;
	new->bp = head->bp;
	head->bp->fp = new;
	head->bp = new;
}

void rmclient(struct client *c)
{
	c->fp->bp = c->bp;
	c->bp->fp = c->fp;
	c->fp = c->bp = NULL;
}

struct client *search_client(struct client *head, in_addr_t c_id)
{
	struct client *tmp;

	for(tmp = head->fp; tmp != head; tmp = tmp->fp)
		if((tmp->id).s_addr == c_id)
			return tmp;
	
	return NULL;
}

