#include "mydhcp.h"

int main(int argc, char **argv)
{
	int sd;
	char buf[20] = "0123456789AB";
	struct sockaddr_in sr_sock;
	struct in_addr sr_addr;
	
	if(argc < 2){
		printf("Usage: ./mydhcpc server-IP-address\n");
		exit(1);
	}

	if((sd = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
		perror("socket");
		exit(1);
	}
	memset(&sr_addr, 0, sizeof(struct in_addr));
	inet_aton(argv[1], &sr_addr); 
	sr_sock.sin_family = AF_INET;
	sr_sock.sin_port = htons(MYPORT);
	sr_sock.sin_addr.s_addr = htonl(sr_addr.s_addr);
	buf[0] = '\x00';	
	while(1){
		sleep(1);
		buf[0]++;
		sendto(sd, buf, 20, 0, (struct sockaddr *)&sr_sock, sizeof(sr_sock));
	}
	return 0;
}
