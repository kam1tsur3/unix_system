#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "main.h"

void help_proc(int argc, char** argv)
{
	//printf
	printf("-----Help Menu-----\n\n");
	printf("** COMMAND LIST ** \n\n");
	printf("  init  :");
	printf(" Initialize hashlist and freelist.\n\n");
	printf("  buf [n ...]  :");
	printf(" Show the statuses of all buffers. If you set number of buffer(0~CACHENUM) as arguments, you can see the status of the specified buffer.\n\n");
	printf("  hash [n ...]  :");
	printf(" Show the status of buffers in hash number order. If you set number of buffer(0~NHASH) as argument, you can see the status of the buffers whose hash is specified number.\n\n");
	printf("  free  :");
	printf(" Show the status of buffers that is in freelist.\n\n");
	printf("  getblk <n>  :");
	printf(" Lock the buffer. You have to set logical block number as argument.\n\n");
	printf("  brelse <n>  :");
	printf(" Unlock the buffer. You have to set logical block number as argument.\n\n");
	printf("  set <n> <stat> [stat ...]  :");
	printf(" Set the status of buffer. You have to set the logical block number as first argument and the status of buffer you want to set after second argument.\n\n");
	printf("  reset <n> <stat> [stat ...]  :");
	printf(" Unset the status of buffer. You have to set the logical block number as first argument and the status of buffer you want to unset after second argument.\n\n");
	printf("  quit  :");
	printf(" Exit from this program.\n\n");
	printf("** STATUS LIST ** \n\n");
	printf("If you run set command or reset command, at least one of the following capital letters should be set argument.\n\n");
	printf("  L : STAT_LOCKED\n\n");
	printf("  V : STAT_VALID\n\n");
	printf("  D : STAT_DWR\n\n");
	printf("  K : STAT_KRDWR\n\n");
	printf("  W : STAT_WAITED\n\n");
	printf("  O : STAT_OLD\n\n");

	return;
}

void init_proc(int argc, char** argv)
{
	if(argc > 1){
		printf("Invalid argument.\n");
		return;
	}
	int i;
	struct buf_header* buf;
	int hashedlist[12] = {28, 4, 64, 17, 5, 97,
						98, 50, 10, 3, 35, 99};
	int freelist[6] = {3, 5, 4, 28, 97, 10};
	for(i = 0; i < NHASH; i++){
		buf = &hash_head[i];
		buf->blkno = 0;
		buf->hash_fp = buf;
		buf->hash_bp = buf;
		buf->stat = 0;
		buf->free_fp = NULL;
		buf->free_bp = NULL;
		buf->cache_data = NULL;
	}
	free_head.blkno = 0;
	free_head.hash_fp = free_head.hash_bp = NULL;
	free_head.stat = 0;
	free_head.free_fp = free_head.free_bp = &free_head;
	free_head.cache_data = NULL;

	for(i = 0; i < CACHENUM; i++){
		int h = hash(hashedlist[i]);
		buf = &header_list[i];
		insert_bottom_hash(&hash_head[h], buf);
		buf->blkno = hashedlist[i];
		buf->stat = STAT_VALID | STAT_LOCKED;
		buf->free_fp = buf->free_bp = NULL;
		if(init_flag == 0)
			buf->cache_data = (char*)malloc(1024);
		if(buf->cache_data = NULL){
			printf("ERROR AT MALLOC IN INIT\n");
			exit(1);
		}
	}

	for(i = 0; i < 6; i++){
		buf = hash_search(freelist[i]);
		if(buf == NULL){
			printf("ERROR AT INIT\n");
			exit(1);
		}
		insert_bottom_free(&free_head, buf);
		buf->stat &= ~STAT_LOCKED;
	}
	printf("Successfully initialized.\n");
	init_flag = 1;
	return;	
}

void buf_proc(int argc, char** argv)
{
	if(init_flag == 0){
		printf("You should run init command first.\n");
		return;
	}

	char test;
	int i, num;
	struct buf_header* tmp;
	if(argc == 1){
		for(i = 0; i < CACHENUM; i++){
			tmp = &header_list[i];
			print_stat(tmp);
			printf("\n");
		}
		return;
	}
	for(i = 1; i < argc; i++){
		test = *argv[i];
		num = atoi(argv[i]);
		if(test < '0' || test > '9' || num >= CACHENUM){
			printf("Invalid buffer number.\n");
			continue;
		}
		tmp = &header_list[num];
		print_stat(tmp);
		printf("\n");
	}
	return;	
}

void hash_proc(int argc, char** argv)
{
	if(init_flag == 0){
		printf("You should run init command first.\n");
		return;
	}

	char test;
	int i, num; 
	struct buf_header* tmp;
	if(argc == 1){
		for(i = 0; i < NHASH; i++){
			printf("%d: ", i);
			for(tmp = hash_head[i].hash_fp; tmp != &hash_head[i]; tmp = tmp->hash_fp){
				print_stat(tmp);
				putchar(' ');
			}
			putchar('\n');
		}
		return;
	}
	for(i = 1; i < argc; i++){
		test = *argv[i];
		num = atoi(argv[i]);
		if(test < '0' || test > '9' || num >= NHASH){
			printf("Invalid buffer number.\n");
			continue;
		}
		printf("%d: ", num);
		for(tmp = hash_head[num].hash_fp; tmp != &hash_head[num]; tmp = tmp->hash_fp){
			print_stat(tmp);
			putchar(' ');
		}
		putchar('\n');
	}
	return;
}

void free_proc(int argc, char** argv)
{
	if(init_flag == 0){
		printf("You should run init command first.\n");
		return;
	}

	struct buf_header* tmp;
	if(argc > 1){
		printf("free command doesn't need any argument.\n");
		return;
	}
	for(tmp = free_head.free_fp; tmp != &free_head; tmp = tmp->free_fp){
		print_stat(tmp);
		putchar(' ');
	}
	putchar('\n');
	return;
}

void getblk_proc(int argc, char** argv)
{
	if(init_flag == 0){
		printf("You should run init command first.\n");
		return;
	}

	if(argc != 2){
		printf("Invalid argument.\n");
		printf("Usage:$getblk <blocknumber>\n");
		return;
	}
	int blkno;
	blkno = atoi(argv[1]);
	if(blkno == 0){
		printf("Invalid argument.\n");
		return;
	}
	getblk(blkno);
	return;
}

void brelse_proc(int argc, char** argv)
{
	if(init_flag == 0){
		printf("You should run init command first.\n");
		return;
	}

	if(argc != 2){
		printf("Invalid argument.\n");
		printf("Usage:$brelse <blocknumber>\n");
		return;
	}
	int blkno;
	struct buf_header* buffer;
	blkno = atoi(argv[1]);
	if(blkno == 0){
		printf("Invalid logical block number.\n");
		return;
	}
	buffer = hash_search(blkno);
	if(buffer == NULL){
		printf("The buffer %d is not on cache.\n", blkno);
		return;
	}
	brelse(buffer);
	return;
}

void set_proc(int argc, char** argv)
{	
	if(init_flag == 0){
		printf("You should run init command first.\n");
		return;
	}

	char* tmp_argv;
	char stat_char[6] = {'O', 'W', 'K', 'D', 'V', 'L'};
	struct buf_header* tmp_buf;	
	int blkno, i, j, tmp_stat;
	if(argc < 3){
		printf("Invalid argument\n");
		printf("Usage:$set <logical block number> [status]\n");
		return;
	}

	blkno = atoi(argv[1]);
	if(blkno == 0){
		printf("Invalid logical block number.\n");
		return;
	}

	tmp_buf = hash_search(blkno);
	if(tmp_buf == NULL){
		printf("Invalid logical block number.\n");
		return;
	}

	for(i = 2; i < argc; i++){
		for(tmp_argv = argv[i]; *tmp_argv != '\x00'; tmp_argv++){
			for(j = 0; j < NSTAT; j++){
				if(*tmp_argv == stat_char[j]){
					tmp_stat = STAT_OLD >> j;
					if(tmp_stat == STAT_LOCKED){
						printf("Please lock with getblk command.\n");
						break;
					}
					tmp_buf->stat |= tmp_stat;
					break;
				}
			}
		}
	}
	printf("Set the buffer status.\n");
	print_stat(tmp_buf);
	putchar('\n');
	return;
}

void reset_proc(int argc, char** argv)
{
	if(init_flag == 0){
		printf("You should run init command first.\n");
		return;
	}

	char *tmp_argv;
	char stat_char[6] = {'O', 'W', 'K', 'D', 'V', 'L'};
	struct buf_header* tmp_buf;	
	int blkno, i, j, tmp_stat;
	
	if(argc < 3){
		printf("Invalid argument\n");
		printf("Usage:$reset <logical block number> [status]\n");
		return;
	}

	blkno = atoi(argv[1]);
	if(blkno == 0){
		printf("Invalid logical block number.\n");
		return;
	}

	tmp_buf = hash_search(blkno);
	if(tmp_buf == NULL){
		printf("Invalid logical block number.\n");
		return;
	}

	for(i = 2; i < argc; i++){
		for(tmp_argv = argv[i]; *tmp_argv != '\x00'; tmp_argv++){
			for(j = 0; j < NSTAT; j++){
				if(*tmp_argv == stat_char[j]){
					tmp_stat = STAT_OLD >> j;
					if(tmp_stat == STAT_LOCKED){
						printf("Please unlock with brelse command.\n");
						break;
					}
					if(tmp_stat == STAT_VALID){
						printf("You can't unset VALID flag with reset command.\n");
						break;
					}	
					tmp_buf->stat &= ~tmp_stat;
					break;
				}
			}
		}
	}
	printf("Reset the buffer status.\n");
	print_stat(tmp_buf);
	putchar('\n');
	return;
}

void quit_proc(int argc, char** argv)
{
	int i;	
	if(argc > 1){
		printf("Invalid argument.\n");
		return;
	}
	if(init_flag == 1)
		for(i = 0; i < CACHENUM; i++)
			free(header_list[i].cache_data);
	printf("Exit.\n");
	exit(0);
}
