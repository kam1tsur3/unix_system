#include "mydhcp.h"

struct sockaddr_in sr_sock;
struct sockaddr_in r_sock;
struct in_addr my_ip, my_netmask;
int sd;
char alrm_flag;
void hup_handler()
{
	char s_buf[100];
	memset(&s_buf, 0, 100);
	s_buf[0] = '\x05';
	*((in_addr_t *)&s_buf[4]) = my_ip.s_addr;
	sendto(sd, s_buf, 100, 0, (struct sockaddr *)&sr_sock, sizeof(sr_sock));
	printf("Send RELEASE\n\n");
	exit(0);
}

void alrm_handler()
{
	alrm_flag = 1;	
}

int main(int argc, char **argv)
{
	int status;
	char s_buf[100], r_buf[100], again, type, code;
	short ttl;
	struct in_addr sr_addr;
	fd_set rdfds;	
	struct timeval timeout;
	struct itimerval timer;
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
	printf("------------------------------\n");	
	printf("Send DISCOVER\n\n");
	printf("Status: CLOSE -> OFFER wait\n\n");
	status = 1;
	alrm_flag = 0;
	signal(SIGHUP, hup_handler);
	signal(SIGALRM, alrm_handler);
	
	while(1){
		memset(s_buf, 0, 100);
		memset(r_buf, 0, 100);
		timeout.tv_sec = TIMEOUT;
		timeout.tv_usec = 0;
		if(status == 3){
			pause();
			if(alrm_flag == 1){
				s_buf[0] = '\x03';
				s_buf[1] = '\x03';
				*((short *)&s_buf[2]) = ttl;
				*((in_addr_t *)&s_buf[4]) = my_ip.s_addr;
				*((in_addr_t *)&s_buf[8]) = my_netmask.s_addr;
				sendto(sd, s_buf, 100, 0, (struct sockaddr *)&sr_sock, sizeof(sr_sock));
				printf("------------------------------\n");	
				printf("Send REQUEST Code = 3\n\n");	
				status = 2;
			}	
			continue;
		}
		if(select(sd+1, &rdfds, NULL, NULL, &timeout) == 0){
			if(again == 1){
				printf("Time out\n\n");
				exit(1);
			}
			switch(status){
				case 1:
					s_buf[0] = '\x01';
					sendto(sd, s_buf, 100, 0, (struct sockaddr *)&sr_sock, sizeof(sr_sock));
					printf("------------------------------\n");	
					printf("Send DISCOVER (again)\n\n");	
					again = 1;
					fflush(stdin);
					FD_ZERO(&rdfds);
					FD_SET(0, &rdfds);
					FD_SET(sd, &rdfds);
					break;
				case 2:
					s_buf[0] = '\x03';
					if(alrm_flag == 0)
						s_buf[1] = '\x02';
					else if(alrm_flag == 1)
						s_buf[1] = '\x03';
					*((short *)&s_buf[2]) = ttl;
					*((in_addr_t *)&s_buf[4]) = my_ip.s_addr;
					*((in_addr_t *)&s_buf[8]) = my_netmask.s_addr;
					sendto(sd, s_buf, 100, 0, (struct sockaddr *)&sr_sock, sizeof(sr_sock));
					printf("------------------------------\n");	
					printf("Send REQUEST Code=%d (again)\n\n", alrm_flag+2);
					again = 1;
					fflush(stdin);
					FD_ZERO(&rdfds);
					FD_SET(0, &rdfds);
					FD_SET(sd, &rdfds);
					break;
			}
			continue;
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
					printf("------------------------------\n");	
					printf("Receive OFFER Code=0\n\n");
					printf("Status: OFFER wait -> ACK wait\n\n");
					printf("Send REQUEST Code=%d\n\n", alrm_flag+2);
				}
				else if(code == '\x01'){
					printf("------------------------------\n");	
					printf("Recieve OFFER Code=1\n\n");
					exit(1);
				}
				else{
					printf("------------------------------\n");	
					printf("Invalid code\n\n");
					exit(1);
				}
			}
			else if(type == '\x04' && status == 2){
				if(code == '\x00'){
					my_ip.s_addr = tmp_ip;
					my_netmask.s_addr = tmp_netmask;
					again = 0;
					printf("------------------------------\n");	
					printf("Recieve ACK Code=0\n\n");
					printf("Status: ACK wait -> IP GET\n\n");
					printf("IP addr: %s, ", inet_ntoa(my_ip));
					printf("Netmask: %s, ", inet_ntoa(my_netmask));
					printf("ttl %hd\n\n", ttl);
					status = 3;
					timer.it_interval.tv_sec = 0;
					timer.it_interval.tv_usec = 0;
					timer.it_value.tv_sec = (ttl+1)/2;
					timer.it_value.tv_usec = 0;
					setitimer(ITIMER_REAL, &timer, NULL);
				}
				else if(code == '\x04'){
					printf("------------------------------\n");	
					printf("Recieve ACK Code=4\n\n");
					exit(1);
				}
			}
			else{
				printf("------------------------------\n");	
				printf("Invalid type\n\n");
				exit(1);
			}
			again = 0;
			alrm_flag = 0;
		}
	}
	return 0;
}
