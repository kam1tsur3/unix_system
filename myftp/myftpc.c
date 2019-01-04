#include "myftp.h"

const char *ins_table[10] = {"quit", "pwd", "cd", "dir", "lpwd", "lcd", "ldir", "get", "put", "help"};

char *exe_table[10] = {NULL, NULL, NULL, NULL, "/bin/pwd", NULL, "/bin/ls", NULL, NULL, NULL};
char *option = "-l";

int main(int argc, char **argv)
{
	int sd, ins_num, i, c_status, fd;
	pid_t pid;
	struct sockaddr_in sr_sock;
	struct in_addr sr_addr;
	struct myftph header;
	char s_buf[DATASIZE], r_buf[DATASIZE];
	char ins_buf[BUF_SIZE];
	char *p_argv[ARG_SIZE];
	int p_argc;

	if(argc < 2){
		printf("Usage: ./myftpc server-IP-address\n");
		exit(1);
	}

	if((sd = socket(PF_INET, SOCK_STREAM, 0)) < 0){
		perror("socket");
		exit(1);
	}
	
	memset(&sr_sock, 0, sizeof(sr_sock));
	inet_aton(argv[1], &sr_addr);
	sr_sock.sin_family = AF_INET;
	sr_sock.sin_port = htons(MYPORT);
	sr_sock.sin_addr.s_addr = htonl(sr_addr.s_addr);

	if(connect(sd, (struct sockaddr *)&sr_sock, sizeof(sr_sock)) < 0){
		perror("connect");
		exit(1);
	}
	memset(s_buf, 0, DATASIZE);
	memset(r_buf, 0, DATASIZE);

	while(1){
		memset(ins_buf, 0, BUF_SIZE);
		memset(p_argv, 0, sizeof(char *)*ARG_SIZE);
		printf("myFTP%% ");
		fgets(ins_buf, BUF_SIZE, stdin);
		p_argc = analysys_args(p_argv, ins_buf);
		for(ins_num = 0; ins_num < 10; ins_num++){
			if(strcmp(p_argv[0], ins_table[ins_num]) == 0)
				break;
		}
		switch(ins_num){
			case 0:
				header.type = 1;
				header.code = 0;
				header.length = 0;
				send(sd, (char *)&header, HEADERSIZE, 0);
				printf("Send QUIT\n");
				recv(sd, (char *)&header, HEADERSIZE, 0);
				printf("Recieve OK\n");
				printf("Exit.\n");
				exit(0);
				break;
			case 1:
				memset(r_buf, 0, DATASIZE);
				header.type = 2;
				header.code = 0;
				header.length = 0;
				send(sd, (char *)&header, HEADERSIZE, 0);
				printf("Send PWD\n");	
				recv(sd, (char *)&header, HEADERSIZE, 0);
				recv(sd, r_buf, header.length, 0);
				printf("%s\n", r_buf);
				break;
			case 2:
				if(p_argc < 2){
					printf("Usage: cd <directory name>\n");
					break;
				}
				header.type = 3;
				header.code = 0;
				header.length = strlen(p_argv[1]);
				send(sd, (char *)&header, HEADERSIZE, 0);
				send(sd, p_argv[1], strlen(p_argv[1]), 0);
				recv(sd, (char *)&header, HEADERSIZE, 0);
				if(err_check(header.type, header.code) == 0)
					printf("Changed working directory on server\n");	
				break;
			case 3:
				header.type = 4;
				header.code = 0;
				if(p_argc < 2){
					header.length = 0;
					send(sd, (char *)&header, HEADERSIZE, 0);
				}	
				else{
					header.length = strlen(p_argv[1]);
					send(sd, (char *)&header, HEADERSIZE, 0);
					send(sd, p_argv[1], header.length, 0);
				}
				recv(sd, (char *)&header, HEADERSIZE, 0);
				if(err_check(header.type, header.code) == 0){
					while(header.code == 1){
						memset(r_buf, 0, DATASIZE);
						recv(sd, (char *)&header, HEADERSIZE, 0);
						recv(sd, r_buf, header.length, 0);
						write(1, r_buf, header.length);
					}
				}
				break;	
			case 5:
				if(p_argc < 2)
					printf("Usage: lcd <directory name>\n");
				else if(chdir(p_argv[1]) < 0){
					if(errno == ENOENT)		
						printf("No such a file or directory\n");
					else if(errno == EACCES)
						printf("Permission denied\n");
					else
						printf("Unknown error\n");
				}
				break;	
			case 6:
				p_argv[2] = p_argv[1];
				p_argv[1] =	option;
			case 4:
				p_argv[0] = exe_table[ins_num];
				if((pid = fork()) < 0){
					perror("fork");
					exit(1);
				}
				else if(pid == 0)
					execve(exe_table[ins_num], p_argv, NULL);
				else
					wait(&c_status);	
				break;
			case 7:
				if(p_argc < 3){
					printf("Usage: get <filename on server> <filename on client>\n\n");
					break;
				}
				else{
					fd = open(p_argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
					if(fd < 0){
						perror("open");
						break;
					}
					else{
						header.type = 5;
						header.code = 0;
						header.length = strlen(p_argv[1]);
						send(sd, (char *)&header, HEADERSIZE, 0);
						send(sd, p_argv[1], header.length, 0);
						recv(sd, (char *)&header, HEADERSIZE, 0);
						if(err_check(header.type, header.code) == 0){
							while(header.code){
								memset(r_buf, 0, DATASIZE);
								recv(sd, (char *)&header, HEADERSIZE, 0);
								recv(sd, r_buf, header.length, 0);
								write(fd, r_buf, header.length);
							}
						}
						close(fd);
					}
				}
				break;
			case 8:
				if(p_argc < 3){
					printf("Usage: put <filename on client> <filename on server>\n\n");
					break;
				}
				else{
					fd = open(p_argv[1], O_RDONLY);
					if(fd == -1){
						if(errno == ENOENT)
							printf("No such a file or directory\n");
						else if(errno == EACCES)
							printf("Permission denied\n");
						else 
							printf("Unknown error\n");
						break;
					}
					else{
						header.type = 6;
						header.code = 0;
						header.length = strlen(p_argv[2]);
						send(sd, (char *)&header, HEADERSIZE, 0);
						send(sd, p_argv[2], header.length, 0);
						recv(sd, (char *)&header, HEADERSIZE, 0);
						if(err_check(header.type, header.code) == 0){
							while(header.code != 0){
								memset(s_buf, 0, DATASIZE);
								header.type = 0x20;
								header.length = read(fd, s_buf, DATASIZE);
								printf("read %hd\n", header.length);
								if(header.length == DATASIZE)
									header.code = 0x01;
								else 
									header.code = 0x00;
								send(sd, (char *)&header, HEADERSIZE, 0);
								send(sd, s_buf, header.length, 0);	
							}
						}
						close(fd);
						break;
					}
				}
				break;
			case 9:
				printf("------MYFTPC HELP------\n\n");
				printf("* quit : Close myFTP service\n\n");
				printf("* pwd  : Show the current working directory on server\n\n");
				printf("* cd   : Change the current working directory on server\n");
				printf(" -Usage: cd <directory name>\n\n");
				printf("* dir  : Show files or directory in the current working directory on server\n");
				printf(" -Usage: dir [file name | directory name]\n\n");
				printf("* lpwd : Show the current working directory on client\n\n");
				printf("* lcd  : Change the current working directory on client\n");
				printf(" -Usage: lcd <directory name>\n\n");
				printf("* ldir : Show files or directory in the current working directory on client\n");
				printf(" -Usage: ldir [file name | directory name]\n\n");
				printf("* get  : Transfer a file from server to client\n");
				printf(" -Usage: get <filename on server> <filename on client>\n\n");
				printf("* put  : Transfer a file from client to server\n");
				printf(" -Usage: put <filename on client> <filename on server>\n\n");
				break;
			default:
				printf("Not implemented command\n");
		}
	}			
	
	return 0;
}
