#include <stdio.h>
#include "main.h"

int hash(int blkno)
{
	return blkno%4;
}
struct buf_header* hash_search(int blkno)
{
	int h;
	struct buf_header *p;

	h = hash(blkno);
	for(p = hash_head[h].hash_fp; p != &hash_head[h]; p = p->hash_fp)
		if(p->blkno == blkno)
			return p;

	return NULL;
}

void insert_head_hash(struct buf_header *h, struct buf_header *p)
{
	struct buf_header* fp = h->hash_fp;
	p->hash_bp = h;
	p->hash_fp = fp;
	fp->hash_bp = p;
	h->hash_fp = p;
	return;
}

void insert_bottom_hash(struct buf_header *h, struct buf_header *p)
{
	struct buf_header* bp = h->hash_bp;
	p->hash_fp = h;
	p->hash_bp = bp;
	h->hash_bp = p;
	bp->hash_fp = p;
	return;
}

void insert_head_free(struct buf_header *h, struct buf_header *p)
{
	if(p->stat & STAT_LOCKED == 0){
		printf("Buffer is not locked.\n");
		return;
	}
	struct buf_header* fp = h->free_fp;
	p->free_bp = h;
	p->free_fp = fp;
	fp->free_bp = p;
	h->free_fp = p;
	return;
}

void insert_bottom_free(struct buf_header *h, struct buf_header *p)
{
	if(p->stat & STAT_LOCKED == 0){
		printf("Buffer is not locked.\n");
		return;
	}
	struct buf_header* bp = h->free_bp;
	p->free_fp = h;
	p->free_bp = bp;
	h->free_bp = p;
	bp->free_fp = p;

	return;
}
void remove_hash(struct buf_header *p)
{
	struct buf_header *tmp;
	tmp = p->hash_fp;
	tmp->hash_bp = p->hash_bp;
	tmp->hash_bp->hash_fp = tmp;
	p->hash_fp = p->hash_bp = NULL;
	return;
}

void remove_free(struct buf_header *p)
{
	struct buf_header *tmp;
	tmp = p->free_fp;
	tmp->free_bp = p->free_bp;
	tmp->free_bp->free_fp = tmp;
	p->free_fp = p->free_bp = NULL;
	p->stat |= STAT_LOCKED;
	return;
}

void print_stat(struct buf_header *p)
{
	int i, buf_num, stat_tmp;
	stat_tmp = STAT_OLD;
	buf_num = (int)((unsigned long)p - (unsigned long)header_list) / (int)(sizeof(struct buf_header));
	char status_char[6] = {'O', 'W', 'K', 'D', 'V', 'L'};
	printf("[%2d:%3d ", buf_num, p->blkno);
	for(i = 0; i < 6; i++){
		if((stat_tmp & p->stat) == 0)
			putchar('-');
		else
			printf("%c", status_char[i]);
		stat_tmp >>= 1;	
	}
	putchar(']');
	return;
}

