#include "report3.h"

void newpro(int argc, char** argv)
{
	int* status;
	pid_t pid;
	
	if(){//argv has pipe token
		if((pid = fork()) < 0){
			perror("fork");
			exit(1);
		}
		else if(pid == 0){
			newpro();	
		}
		else{
			//exec
			wait(status);
			exit(0);
		}
	}
	else if(){
		//argv has redirect token
	}
	else{
		//exec
		exit(0);
	}
}
