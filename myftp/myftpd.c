#include "myftp.h"

int main(int argc, char **argv)
{
	int sd, sub_sd, fd, status;
	socklen_t s_len;
	pid_t pid, pid2;
	struct sockaddr_in mysock;
	struct sockaddr_in c_sock;
	struct myftph header;
	char s_buf[PACKSIZE], r_buf[PACKSIZE];
	char *ls_list[4] = {"/bin/ls", "-l", NULL, NULL};
	if((sd = socket(PF_INET, SOCK_STREAM, 0)) < 0){
		perror("socket");
		exit(1);
	}

	memset(&mysock, 0, sizeof(mysock));
	mysock.sin_family = AF_INET;
	mysock.sin_port = htons(MYPORT);
	mysock.sin_addr.s_addr = htonl(INADDR_ANY);

	if(bind(sd, (struct sockaddr *)&mysock, sizeof(mysock)) < 0){
		perror("bind");
		exit(1);
	}

	if(listen(sd, 5) < 0){
		perror("listen");
		exit(1);
	}
	s_len = sizeof(c_sock);
	
	while(1){	
		if((sub_sd = accept(sd, (struct sockaddr *)&c_sock, &s_len)) < 0){
			perror("accept");
			exit(1);
		}
		if((pid = fork()) < 0){
			perror("fork");
			exit(1);
		}
		else if(pid == 0){
			break;
		}
		else
			continue;
	}
	
	while(1){
		errno = 0;
		if(recv(sub_sd, (char *)&header, HEADERSIZE, 0) < 0)
			exit(0);
		switch(header.type){
			case 1:
				printf("Recieve QUIT\n");	
				header.type = 0x10;
				header.code = 0;
				header.length = 0;
				send(sub_sd, (char *)&header, HEADERSIZE, 0);
				printf("Send OK\n");
				close(sub_sd);
				exit(0);
				break;
			case 2:
				printf("Recieve PWD\n");
				memset(s_buf, 0, DATASIZE);
				getcwd(s_buf, DATASIZE);
				header.type = 0x10;
				header.code = 0;
				header.length = strlen(s_buf);
				send(sub_sd, (char *)&header, HEADERSIZE, 0);
				send(sub_sd, s_buf, header.length, 0);
				printf("Send OK\n");
				break;
			case 3:
				printf("Recieve CWD\n");
				memset(r_buf, 0, DATASIZE);
				recv(sub_sd, r_buf, header.length, 0);
				if(chdir(r_buf) < 0){
					if(errno == ENOENT){
						header.type = 0x12;
						header.code = 0x00;
						header.length = 0;
						send(sub_sd, (char *)&header, HEADERSIZE, 0);
					}
					else if(errno == EACCES){
						header.type = 0x12;
						header.code = 0x01;
						header.length = 0;
						send(sub_sd, (char *)&header, HEADERSIZE, 0);
					}
					else{
						header.type = 0x13;
						header.code = 0x05;
						header.length = 0;
						send(sub_sd, (char *)&header, HEADERSIZE, 0);
					}				
				}
				else{
					header.type = 0x10;
					header.code = 0;
					header.length = 0;
					send(sub_sd, (char *)&header, HEADERSIZE, 0);
				}
				break;
			case 4:
				printf("Recieve LIST\n");
				if(header.length > 0){
					memset(r_buf, 0, DATASIZE);
					recv(sub_sd, r_buf, header.length, 0);
					ls_list[2] = r_buf;
				}
				else{
					ls_list[2] = NULL;
				}
				fd = open(TMPFILE, O_CREAT | O_RDWR | O_TRUNC, 0644);
				if(fd < 0){
					perror("open");
					header.type = 0x13;
					header.code = 0x5;
					header.length = 0;
					send(sub_sd, (char *)&header, HEADERSIZE, 0);
					break;
				}
				pid2 = fork();
				if(pid2 < 0){
					perror("fork");
					header.type = 0x13;
					header.code = 0x5;
					header.length = 0;
					send(sub_sd, (char *)&header, HEADERSIZE, 0);
					break;
				}
				else if(pid2 == 0){
					dup2(fd, 1);
					execve(ls_list[0], ls_list, NULL);
					close(fd);
					exit(0);
				}
				else{
					wait(&status);
					header.type = 0x10;
					header.code = 0x01;
					header.length = 0x00;
					send(sub_sd, (char *)&header, HEADERSIZE, 0);
					lseek(fd, 0, 0);
					while(header.code != 0){
						memset(s_buf, 0, DATASIZE);
						header.type = 0x20;
						header.length = read(fd, s_buf, DATASIZE);
						if(header.length == DATASIZE)
							header.code = 0x01;
						else
							header.code = 0x00;
						send(sub_sd, (char *)&header, HEADERSIZE, 0);
						send(sub_sd, s_buf, header.length, 0);
					}
					remove(TMPFILE);
					close(fd);
				}
				break;
			case 5:
				printf("Recieve RETR\n");
				memset(r_buf, 0, DATASIZE);
				recv(sub_sd, r_buf, header.length, 0);
				fd = open(r_buf, O_RDONLY);
				if(fd < 0){
					if(errno == ENOENT){
						header.type = 0x12;
						header.code = 0x00;
						header.length = 0;
						send(sub_sd, (char *)&header, HEADERSIZE, 0);
					}
					else if(errno == EACCES){
						header.type = 0x12;
						header.code = 0x01;
						header.length = 0;
						send(sub_sd, (char *)&header, HEADERSIZE, 0);
					}
					else{
						header.type = 0x13;
						header.code = 0x05;
						header.length = 0;
						send(sub_sd, (char *)&header, HEADERSIZE, 0);
					}
					break;
				}
				else{
					header.type = 0x10;
					header.code = 0x01;
					header.length = 0x00;
					send(sub_sd, (char *)&header, HEADERSIZE, 0);
					while(header.code != 0x00){
						memset(s_buf, 0, DATASIZE);
						header.type = 0x20;
						header.length = read(fd, s_buf, DATASIZE);
						if(header.length == DATASIZE)
							header.code = 0x01;
						else
							header.code = 0x00;
						send(sub_sd, (char *)&header, HEADERSIZE, 0);
						send(sub_sd, s_buf, header.length, 0);
					}
					close(fd);
				}
				break;
			case 6:
				printf("Recieve STOR\n");
				memset(r_buf, 0, DATASIZE);
				recv(sub_sd, r_buf, header.length, 0);
				fd = open(r_buf, O_WRONLY | O_CREAT | O_TRUNC, 0644);
				if(fd < 0){
					perror("open");
					header.type = 0x13;
					header.code = 0x05;
					header.length = 0;
					send(sub_sd, (char *)&header, HEADERSIZE, 0);
					break;
				}
				else{
					header.type = 0x10;
					header.code = 0x02;
					header.length = 0;
					send(sub_sd, (char *)&header, HEADERSIZE, 0);
					while(header.code != 0){
						memset(r_buf, 0, DATASIZE);
						recv(sub_sd, (char *)&header, HEADERSIZE, 0);
						recv(sub_sd, r_buf, header.length, 0);
						write(fd, r_buf, header.length);
					}
					close(fd);
				}
				break;
			default:
				printf("Not implemented\n");
				header.type = 0x11;
				header.code = 0x02;
				header.length = 0;
				send(sub_sd, (char *)&header, HEADERSIZE, 0);
				break;
		}
	}
	return 0;
}

