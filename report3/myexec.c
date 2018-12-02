#include "report3.h"

int myexec(int argc, char **argv)
{
	int i, index, fd, size, exesize, ret_stat;
	char *path, *ptr;
	char path_buf[BUF_SIZE];
	char **tmp_addr;
	char *token_addr;
	
	struct stat status;
	
	tmp_addr = argv;
	while(1){
		index = search_redirect(argc, tmp_addr);
		if(index == -1){
			break;
		}
		else if(index >= 2*ARG_SIZE){
			index -= 2*ARG_SIZE;
			fd = open(tmp_addr[index+1], O_RDONLY);
			if(fd == -1){
				fprintf(stderr, "open error\n");
				exit(1);
			}
			close(0);
			dup(fd);

			tmp_addr[index] = NULL;
			tmp_addr = &tmp_addr[index+1];
			argc -= index+1;
		}
		else if(index >= ARG_SIZE){
			index -= ARG_SIZE;
			fd = open(tmp_addr[index+1], O_WRONLY | O_CREAT | O_APPEND, 0644);
			if(fd == -1){
				fprintf(stderr, "open error\n");
				exit(1);
			}
			close(1);
			dup(fd);

			tmp_addr[index] = NULL;
			tmp_addr = &tmp_addr[index+1];
			argc -= index+1;
		}
		else if(index >= 1){
			fd = open(tmp_addr[index+1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
			if(fd == -1){
				fprintf(stderr, "open error\n");
				exit(1);
			}
			close(1);
			dup(fd);

			tmp_addr[index] = NULL;
			tmp_addr = &tmp_addr[index+1];
			argc -= index+1;
		}
	}
	
	if(execve(argv[0], argv, NULL) != -1)
		return 0;
	path = getenv("PATH");
	ptr = path;
	exesize = strlen(argv[0]);
	while(1){
		memset(path_buf, 0, BUF_SIZE);
		if((token_addr = strstr(ptr, ":")) != NULL){
			size = (int)(token_addr - ptr);
			memcpy(path_buf, ptr, size);
			*(path_buf+size) = '/';
			memcpy(path_buf+size+1, argv[0], exesize);
			if((ret_stat = stat(path_buf, &status)) == 0){
				execve(path_buf, &argv[0], NULL);
				break;
			}
			ptr = token_addr;	
			ptr++;
		}
		else{
			size = strlen(ptr);
			memcpy(path_buf, ptr, size);
			*(path_buf+size) = '/';
			memcpy(path_buf+size+1, argv[0], exesize);
			if((ret_stat = stat(path_buf, &status)) == 0){
				execve(path_buf, &argv[0], NULL);
				break;
			}
			fprintf(stderr, "-mysh: %s: command not found\n", argv[0]);
			return -1;
		}
	}
	return 0;
}
