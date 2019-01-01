#include "mydhcp.h"

int main(int argc, char **argv)
{
	int sd, status;
	char s_buf[100], r_buf[100], again, type, code;
	short ttl;
	struct sockaddr_in sr_sock;
	struct sockaddr_in r_sock;
	struct in_addr sr_addr, my_ip, my_netmask;
	fd_set rdfds;	
	struct timeval timeout;
	socklen_t sktlen;
	in_addr_t tmp_ip;
	in_addr_t tmp_netmask;

	timeout.tv_sec = TIMEOUT;
	timeout.tv_usec = 0;

	if(argc < 2){
		printf("Usage: ./mydhcpc server-IP-address\n");
		exit(1);
	}
	//socket init
	if((sd = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
		perror("socket");
		exit(1);
	}

	memset(&sr_addr, 0, sizeof(struct in_addr));
	inet_aton(argv[1], &sr_addr); 
	sr_sock.sin_family = AF_INET;
	sr_sock.sin_port = htons(MYPORT);
	sr_sock.sin_addr.s_addr = htonl(sr_addr.s_addr);

	again = 0;

	FD_ZERO(&rdfds);
	FD_SET(0, &rdfds);
	FD_SET(sd, &rdfds);
	
	memset(s_buf, 0, 100);
	s_buf[0] = '\x01';
	sendto(sd, s_buf, 100, 0, (struct sockaddr *)&sr_sock, sizeof(sr_sock));
	printf("Send DISCOVER\n");
	printf("Status: CLOSE -> OFFER wait\n");
	status = 1;

	while(1){
		memset(s_buf, 0, 100);
		memset(r_buf, 0, 100);
		if(select(sd+1, &rdfds, NULL, NULL, &timeout) == 0){
			if(again == 1){
				printf("Time out\n");
				exit(1);
			}
			switch(status){
				case 1:
					s_buf[0] = '\x01';
					sendto(sd, s_buf, 100, 0, (struct sockaddr *)&sr_sock, sizeof(sr_sock));
					printf("Send DISCOVER (again)\n");	
					again = 1;	
					break;
				case 2:
					s_buf[0] = '\x03';
					s_buf[1] = '\x02';
					*((short *)&s_buf[2]) = ttl;
					*((in_addr_t *)&s_buf[4]) = my_ip.s_addr;
					*((in_addr_t *)&s_buf[8]) = my_netmask.s_addr;
					sendto(sd, s_buf, 100, 0, (struct sockaddr *)&sr_sock, sizeof(sr_sock));
					printf("Send ACK (again)\n");
					again = 1;
					break;
			}
		}
		if(FD_ISSET(sd, &rdfds)){
			recvfrom(sd, r_buf, 100, 0, (struct sockaddr *)&r_sock, &sktlen);
			type = r_buf[0];
			code = r_buf[1];
			ttl = *((short *)&r_buf[2]);
			tmp_ip = *((in_addr_t *)&r_buf[4]);
			tmp_netmask = *((in_addr_t *)&r_buf[8]);
			if(type == '\x02' && status == 1){
				if(code == '\x00'){
					my_ip.s_addr = tmp_ip;
					my_netmask.s_addr = tmp_netmask;
					s_buf[0] = '\x03';
					s_buf[1] = '\x02';
					*((short *)&s_buf[2]) = ttl;
					*((in_addr_t *)&s_buf[4]) = my_ip.s_addr;
					*((in_addr_t *)&s_buf[8]) = my_netmask.s_addr;
					sendto(sd, s_buf, 100, 0, (struct sockaddr *)&sr_sock, sizeof(sr_sock));
					status = 2;
					printf("Receive OFFER Code=0\n");
					printf("Status: OFFER wait -> ACK wait\n");
					printf("Send ACK\n");
				}
				else if(code == '\x01'){
					printf("Recieve OFFER Code=1\n");
					exit(1);
				}
				else{
					printf("Invalid code\n");
					exit(1);
				}
			}
			else if(type == '\x04' && status == 2){
				if(code == '\x00'){
					my_ip.s_addr = tmp_ip;
					my_netmask.s_addr = tmp_netmask;
					again = 0;
					printf("Recieve ACK Code=0\n");
					printf("Status: ACK wait -> IP GET\n");
					printf("IP addr: %s, Netmask: %s, ttl %hd\n", inet_ntoa(my_ip), inet_ntoa(my_netmask), ttl); 
					status = 3;
					break;
				}
				else if(code == '\x04'){
					printf("Recieve ACK Code=4\n");
					exit(1);
				}
			}
			else{
				printf("Invalid type\n");
				exit(1);
			}
			again = 0;
		}
	}
	return 0;
}
