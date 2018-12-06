//61602192 Iyota Shogo
#include "report3.h"

void handler(int sig)
{
	int i;
	int status;
	wait(&status);
}

int main(int argc, char** argv)
{
	char buf[BUF_SIZE];
	char old_buf[BUF_SIZE];
	int i, p_argc, index, fd, tmp_argc;
	pid_t pid, sh_pid, pipe_pid;
	char *p_argv[ARG_SIZE];
	char *tmp_addr, *path;
	char **tmp_argv;
	int status;
	
	int new_fds[2], old_fds[2];
	int pipe_counter;
	int bg_flag;

	signal(SIGINT, SIG_IGN);
	signal(SIGTTOU, SIG_IGN);
	signal(SIGCHLD, SIG_IGN);
	sh_pid = getpid();
	memset(fg_buf, 0, BG_SIZE*sizeof(int));
	fg_head = fg_tail = 0;
	bg_counter = 0;

	while(1)
	{
		memset(buf, 0, BUF_SIZE);
		memset(old_buf, 0, BUF_SIZE);
		for(i = 0; i < ARG_SIZE; i++)
			p_argv[i] = NULL;
		
		putchar('$');
		fgets(old_buf, BUF_SIZE - 1, stdin);
		rebuf(old_buf, buf);
		p_argc = analysys_args(p_argv, buf);
		if(!strcmp(p_argv[p_argc-1], "&")){
			bg_flag = 1;
			p_argv[p_argc-1] = NULL;
			p_argc--;
		}
		else
			bg_flag = 0;

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
							setpgid(pid, pipe_pid);
						}
						else{
							pid = getpid();
							setpgid(pid, pid);
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
						if(bg_flag == 1){
							fd = open("/dev/tty", O_RDWR);
							tcsetpgrp(fd, sh_pid);
							bg_counter = pipe_counter;
							break;
						}
						else{
							fg_buf[fg_tail] = pid;
							fg_tail = (fg_tail + 1) % FG_SIZE;
						}
						for(i = 0; i <= pipe_counter; i++){
							wait(&status);
						}
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
							setpgid(pid, pipe_pid);
						}
						else{
							pid = getpid();
							setpgid(pid, pid);
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
						if(bg_flag != 1){
							fg_buf[fg_tail] = pid;
							fg_tail = (fg_tail + 1) % FG_SIZE;
						}
						if(pipe_counter > 0){
							close(old_fds[0]);
							close(old_fds[1]);
						}
						else
							pipe_pid = pid;
							
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
						wait(&status);
					}
					printf("syntax error\n");
					break;
				}
			}
		}
	}
}

