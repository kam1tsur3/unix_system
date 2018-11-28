#include "report3.h"

int search_token(int argc, char** argv){
	int index;
	int i;

	for(i = 0; i < argc; i++){
		if(!strcmp(p_argv[i], "|")){

		}
		else if(!strcmp(p_argv[i], ">")){

		}
	}
	return -1;
}

void analysys_args(char** p_argv, char* buf){
	
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
		if(*tmp_addr == ' '){
			i--;
			continue;
		}
		p_argv[i] = tmp_addr;
	}
}
