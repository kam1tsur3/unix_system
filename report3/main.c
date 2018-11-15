#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUF_SIZE 	100
void main(int argc, char** argv)
{
	char buf[BUF_SIZE];
	int p_id;
	char pipe_flag;
	char** p_argv;

	while(1)
	{
		puts("$");
		fgets(buf, BUF_SIZE, stdin);



		p_id = fork();
		if(p_id < 0){
			perror("fork");
			exit(1);
		}
		else if(p_id == 0){

		}
		else{

		}
	}
}
