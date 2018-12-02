#include "report3.h"

int main(int argc, char** argv)
{
	char buf[BUF_SIZE];
	int i, p_argc, index, fd, tmp_argc;
	pid_t pid;
	char *p_argv[ARG_SIZE];
	char *tmp_addr, *path;
	char **tmp_argv;
	int *status;
	
	int new_fds[2], old_fds[2];
	int pipe_counter;

	signal(SIGINT, SIG_IGN);
	while(1)
	{
		putchar('$');
		memset(buf, 0, BUF_SIZE);
		for(i = 0; i < ARG_SIZE; i++)
			p_argv[i] = NULL;
		fgets(buf, BUF_SIZE - 1, stdin);
			
		p_argc = analysys_args(p_argv, buf);

		if(!strcmp(p_argv[0], "cd")){
			if(p_argc < 2){
				path = getenv("HOME");
				chdir(path);
			}
			else						//error check
				chdir(p_argv[1]);
			continue;
		}
		else if(!strcmp(p_argv[0], "exit")){
			exit(0);
		}
		else{
			tmp_argv = p_argv;
			tmp_argc = p_argc;
			pipe_counter = 0;

			while(1){
				index = search_pipe(tmp_argc, tmp_argv);
				if(index == -1){
					if((pid = fork()) < 0){
						perror("fork");
						exit(1);
					}
					else if(pid == 0){
						if(pipe_counter > 0){
							close(0);
							dup(old_fds[0]);
							close(old_fds[0]);
							close(old_fds[1]);
						}
						signal(SIGINT, SIG_DFL);
						myexec(tmp_argc, tmp_argv);
						exit(0);
					}
					else{
						if(pipe_counter > 0){
							close(old_fds[0]);
							close(old_fds[1]);
						}
						for(i = 0; i <= pipe_counter; i++)
							wait(status);
						break;	
					}	
				}
				else if(index > 0){
					pipe(new_fds);
					if((pid = fork()) < 0){
						perror("fork");
						exit(1);
					}
					else if(pid == 0){
						if(pipe_counter > 0){
							close(0);
							dup(old_fds[0]);
							close(old_fds[0]);
							close(old_fds[1]);
						}
						tmp_argv[index] = NULL;
						close(1);
						dup(new_fds[1]);
						close(new_fds[0]);
						close(new_fds[1]);
						signal(SIGINT, SIG_DFL);
						myexec(index, tmp_argv);
						exit(0);
					}
					else{
						if(pipe_counter > 0){
							close(old_fds[0]);
							close(old_fds[1]);
						}
						old_fds[0] = new_fds[0];
						old_fds[1] = new_fds[1];
						
						tmp_argc -= (index+1);
						tmp_argv = &tmp_argv[index+1];
						pipe_counter++;
					}
				}
				else{
					if(pipe_counter > 0){
						close(old_fds[0]);
						close(old_fds[1]);
						wait(status);
					}
					printf("syntax error\n");
					break;
				}
			}
		}
	}
}

