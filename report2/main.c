/* 61602192 Shogo Iyota */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "main.h"

struct command_table{
	char *cmd;
	void (*func)(int, char**);
} cmd_tbl[] = {
	{"help", help_proc},
	{"init", init_proc},
	{"buf", buf_proc},
	{"hash", hash_proc},
	{"free", free_proc},
	{"getblk", getblk_proc},
	{"brelse", brelse_proc},
	{"set", set_proc},
	{"reset", reset_proc},
	{"quit", quit_proc},
	{NULL, NULL}
};

int main(/*int argc, char** argv*/)
{
	char tmp_buf[100];
	char *argv_proc[30];
	char *buf_addr;
	int i, argc_proc;
	init_flag = 0;
	while(1){
		memset(tmp_buf, 0, 100);
		putchar('$');
		fgets(tmp_buf, 99, stdin);
		argv_proc[0] = tmp_buf;
		buf_addr = strchr(tmp_buf, '\n');
		if(buf_addr != NULL)
			*buf_addr = 0x00;
		for(i = 1, buf_addr=tmp_buf; i < 29; i++){
			buf_addr = strchr(buf_addr, ' ');
			if(buf_addr == NULL)break;
			*buf_addr = 0x00;
			buf_addr++;
			if(*buf_addr == ' ' || *buf_addr == '\x00'){
				i--;
				continue;
			}
			argv_proc[i] = buf_addr;
		}
		argc_proc = i;
		argv_proc[i] = NULL;
		for(i = 0; cmd_tbl[i].cmd != NULL; i++){
			if(!strcmp(cmd_tbl[i].cmd, argv_proc[0])){
				(cmd_tbl[i].func)(argc_proc, argv_proc);
				break;
			}
		}
		if(cmd_tbl[i].cmd == NULL)
			printf("Command not found. Please see help.\n");
		
	}
	return 0;
}

