#include "report3.h"

int main(int argc, char** argv)
{
	char buf[BUF_SIZE];
	int i;
	pid_t p_id;
	char pipe_flag;
	char* p_argv[ARG_SIZE];
	char* tmp_addr;
	int* status;

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
				exit(0);// remove later
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

