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
	p_argv[i] = NULL;

	return i;
}

void rebuf(char *old_buf, char *new_buf)
{
	int i;
	int ni;
	char c;
	for(i = 0, ni=0; i < BUF_SIZE && ni < BUF_SIZE; i++){
		c = old_buf[i];
		if(c == '<' || c == '|'){
			new_buf[ni] = ' ';
			new_buf[ni+1] = c;
			new_buf[ni+2] = ' ';
			ni += 3;
		}
		else if(c == '>'){
			if(old_buf[i+1] == '>'){
				new_buf[i] = ' ';
				new_buf[i+1] = c;
				new_buf[i+2] = c;
				new_buf[i+3] = ' ';
				ni += 4;
				i++;
			}
			else{
				new_buf[ni] = ' ';
				new_buf[ni+1] = c;
				new_buf[ni+2] = ' ';
				ni +=3;
			}
		}
		else{
			new_buf[ni] = c;
			ni++;
		}
	}
}