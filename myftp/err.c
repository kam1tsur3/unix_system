#include "myftp.h"

int err_check(char type, char code)
{
	switch(type){
		case 0x10:
		case 0x20:
			return 0;
			break;
		case 0x11:
			switch(code){
				case 1:
					printf("ERROR: Syntax error\n");
					return 1;
				case 2:
					printf("ERROR: Undefined command\n");
					return 1;
				case 3:
					printf("ERROR: Protocol error\n");
					return 1;
			}
			break;
		case 0x12:
			switch(code){
				case 0:
					printf("ERROR: No such a file or directory\n");
					return 1;
				case 1:
					printf("ERROR: Permission denied\n");
					return 1;
			}
			break;
		case 0x13:
			if(code == 0x5){
				printf("ERROR: Unknown error\n");
				return 1;
			}
	}
	printf("Undefined Protocol\n");
	return 1;
}
