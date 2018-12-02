#include "report3.h"

int search_pipe(int argc, char** argv)
{
	int index;
	int i;

	for(i = 0; i < argc; i++){
		if(!strcmp(argv[i], "|"))
			return i;
	}	
	return -1;
}

int search_redirect(int argc, char** argv)
{
	int index;
	int i;

	for(i = 0; i < argc; i++){
		if(!strcmp(argv[i], ">"))
			return (i);
		else if(!strcmp(argv[i], ">>"))
			return (ARG_SIZE+i);
		else if(!strcmp(argv[i], "<"))
			return (2*ARG_SIZE+i);
	}
	return -1;
}

int analysys_args(char** p_argv, char* buf)
{
	char* tmp_addr;
	int i;

	p_argv[0] = buf;
	tmp_addr = strchr(buf, '\n');
	if(tmp_addr != NULL)
		*tmp_addr = 0;
	
	for(i = 1, tmp_addr = buf; i < ARG_SIZE - 1; i++){
		tmp_addr = strchr(tmp_addr, ' ');
		if(tmp_addr == NULL)break;
		*tmp_addr = '\x00';
		tmp_addr++;
		if(*tmp_addr == ' ' || *tmp_addr == '\x00'){
			i--;
			continue;
		}
		p_argv[i] = tmp_addr;
	}

	return i;
}
