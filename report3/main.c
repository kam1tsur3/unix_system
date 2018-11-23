#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

#define BUF_SIZE 	100
#define ARG_SIZE	50

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

int main(int argc, char** argv)
{
	char buf[BUF_SIZE];
	int i, p_id;
	char pipe_flag;
	char* p_argv[ARG_SIZE];
	char* tmp_addr;
	int *status;

	while(1)
	{
		putchar('$');
		memset(buf, 0, BUF_SIZE);
		for(i = 0; i < ARG_SIZE; i++)
			p_argv[i] = NULL;
		fgets(buf, BUF_SIZE - 1, stdin);
			
		analysys_args(p_argv, buf);

		if(!strcmp(p_argv[0], "cd")){
			chdir(p_argv[1]);
			continue;
		}
		else if(!strcmp(p_argv[0], "exit")){
			exit(0);
		}
		else{
			p_id = fork();
			if(p_id < 0){
				perror("fork");
				exit(1);
			}
			else if(p_id == 0){
				execvp(p_argv[0], &p_argv[0]);
				exit(0);
			}
			else{
				if(wait(status) == -1){
					perror("wait");
					exit(1);
				}
			}
		}
	}
}
/*
void newpro(int argc, char** argv)
{
	pid_t pid;
	
	if(){//argv has special token
		if((pid = fork()) < 0){
			perror("fork");
			exit(1);
		}
		else if(pid == 0){
			newpro();	
		}
		else{
			//exec
			wait();
			exit(0);
		}
	}
	else{
		//exec
		exit(0);
	}
}
*/
