#include "report3.h"

int main(int argc, char** argv)
{
	char buf[BUF_SIZE];
	int i, p_argc, index, fd;
	pid_t pid;
	char *p_argv[ARG_SIZE];
	char *tmp_addr, *path;
	char **tmp_argv;
	int* status;
	int fds[2];

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
			else
				chdir(p_argv[1]);
			continue;
		}
		else if(!strcmp(p_argv[0], "exit")){
			exit(0);
		}
		else{
			index = search_token(p_argc, p_argv);
			if(index == -1){
				if((pid = fork()) < 0){
					perror("fork");
					exit(1);
				}
				else if(pid == 0){
					execvp(p_argv[0], p_argv);
					exit(0);
				}
				else{
					wait(status);
				}
			}
			else if(index >= ARG_SIZE){
				if((pid = fork()) < 0){
					perror("fork");
					exit(1);
				}
				else if(pid == 0){
					index -= ARG_SIZE;
					fd = open(p_argv[index+1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
					if(fd == -1){
						perror("open");
						exit(1);
					}
					close(1);
					dup(fd);

					p_argv[index] = NULL;
					execvp(p_argv[0], p_argv);
					exit(0);
				}
				else{
					wait(status);
				}
			}
			else if(index >= 0){
				pipe(fds);
				if((pid = fork()) < 0){
					perror("fork");
					exit(1);
				}
				else if(pid == 0){
					p_argv[index] = NULL;
					close(1);
					dup(fds[1]);
					close(fds[0]);
					close(fds[1]);
					execvp(p_argv[0], p_argv);
					exit(0);
				}
				else{
					if((pid = fork()) < 0){
						perror("fork");
						exit(1);
					}
					else if(pid == 0){
						tmp_argv = &p_argv[index+1];
						close(0);
						dup(fds[0]);
						close(fds[0]);
						close(fds[1]);
						execvp(tmp_argv[0], tmp_argv);
						exit(0);
					}
					else{
						close(fds[0]);
						close(fds[1]);
						wait(status);
						wait(status);
					}
				}
			}
		}
	}
}

