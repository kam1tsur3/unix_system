#include "mydhcp.h"

struct ip_pair free_head;
struct client used_head;
int sub_sd;

int main(int argc, char **argv)
{
	FILE *cfd;
	char r_buf[100], s_buf[100], type, code;
	short ttl, t_limit;
	in_addr_t tmp_ip;
	in_addr_t tmp_netmask;
	int i, pair_count, sd;
	in_port_t myport;
	struct sockaddr_in myskt;
	struct sockaddr_in cl_sock;
	socklen_t sktlen;
	struct ip_pair pair_list[PAIR_MAX];
	struct client *tmp_client;
	struct timeval timeout;
	
	used_head.fp = &used_head;
	used_head.bp = &used_head;
	timeout.tv_sec = 10;
	timeout.tv_usec = 0;
	
	if(argc < 2){
		printf("Usage: ./mydhcpd confingfile\n");
		exit(1);
	}

	/* Initializing config file */
	if((cfd = fopen(argv[1], "r")) < 0){
		fprintf(stderr, "fopen error\n");
		return 1;
	}
	
	fscanf(cfd, "%hd", &t_limit);
	memset(pair_list, 0, sizeof(struct ip_pair)*PAIR_MAX);	
	pair_count = 0;
	free_head.fp = &free_head;
	free_head.bp = &free_head;

	for(i = 0; i < PAIR_MAX; i++){
		if(fscanf(cfd, "%s", pair_list[i].ip_addr) == EOF)
			break;
		if(fscanf(cfd, "%s", pair_list[i].netmask) == EOF)
			break;
		addpair_tail(&free_head, &pair_list[i]);
		pair_count++;
	}
	
	//Initializing socket
	if((sd = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
		perror("socket");
		exit(1);
	}
	sub_sd = sd;
	memset(&myskt, 0, sizeof(myskt));
	myskt.sin_family = AF_INET;
	myskt.sin_port = htons(MYPORT);
	myskt.sin_addr.s_addr = htonl(INADDR_ANY);
	if(bind(sd, (struct sockaddr *)&myskt, sizeof(myskt)) < 0){
		perror("bind");
		exit(1);
	}

	while(1){
		memset(r_buf, 0, 100);
		memset(s_buf, 0, 100);
		recvfrom(sd, r_buf, 100, 0, (struct sockaddr*)&cl_sock, &sktlen);
		type = r_buf[0];
		code = r_buf[1];
		ttl = *((short *)&r_buf[2]);
		tmp_ip = *((in_addr_t *)&r_buf[4]);
		tmp_netmask = *((in_addr_t *)&r_buf[8]);
		if(type == '\x01'){
			printf("Recieve DISCOVER\n");
			if(free_head.fp == &free_head){
				s_buf[0] = '\x02';
				s_buf[1] = '\x01';
				sendto(sd, s_buf, 100, 0, (struct sockaddr *)&cl_sock, sizeof(cl_sock));
				printf("Send OFFER Code=1\n");
				printf("Cause: We have no any available IP address\n");
				continue;
			}
			
			if(search_client(&used_head, cl_sock.sin_addr.s_addr) == NULL){
				if((tmp_client = (struct client *)malloc(sizeof(struct client))) == NULL){
					perror("malloc");
					exit(1);
				}
				tmp_client->status = 1;
				tmp_client->ttlcounter = TIMEOUT;
				tmp_client->id.s_addr = cl_sock.sin_addr.s_addr;
				inet_aton(free_head.fp->ip_addr, &(tmp_client->addr));
				inet_aton(free_head.fp->netmask, &(tmp_client->netmask));
				tmp_client->given = free_head.fp;
				tmp_client->ttl = TIMEOUT;
				
				addclient_tail(&used_head, tmp_client);
				rmpair(free_head.fp);
				
				s_buf[0] = '\x02';
				s_buf[1] = '\x00';
				*((short *)&s_buf[2]) = t_limit;
				*((in_addr_t *)&s_buf[4]) = tmp_client->addr.s_addr;
				*((in_addr_t *)&s_buf[8]) = tmp_client->netmask.s_addr;
				sleep(1);
				sendto(sd, s_buf, 100, 0, (struct sockaddr *)&cl_sock, sizeof(cl_sock));
					
				printf("Send OFFER Code=0\n");
				printf("Client %s:DISCOVER wait -> REQUEST wait\n", inet_ntoa(tmp_client->id));
			}
			else{
				s_buf[0] = '\x02';
				s_buf[1] = '\x01';
				sendto(sd, s_buf, 100, 0, (struct sockaddr *)&cl_sock, sizeof(cl_sock));
				printf("Send OFFER Code=1\n");
				printf("Cause: This client has been recieved OFFER\n");
			}
		}
		else if(type == '\x03'){
			printf("Recieve REQUEST\n");
			if((tmp_client = search_client(&used_head, cl_sock.sin_addr.s_addr)) != NULL){
				if(tmp_client->addr.s_addr != tmp_ip || tmp_client->netmask.s_addr != tmp_netmask
					|| ttl > t_limit){
					s_buf[0] = '\x04';
					s_buf[1] = '\x04';
					sendto(sd, s_buf, 100, 0, (struct sockaddr *)&cl_sock, sizeof(cl_sock));
					printf("The client has no permission to use that IP\n"); 	
					printf("Send ACK Code=4\n");
					continue;
				}
				if(code == '\x02'){
					tmp_client->status = 3;
					tmp_client->ttlcounter = ttl;
					tmp_client->ttl = ttl;
					s_buf[0] = '\x04';
					s_buf[1] = '\x00';
					sendto(sd, s_buf, 100, 0, (struct sockaddr *)&cl_sock, sizeof(cl_sock));
					printf("Send ACK Code=0\n");
					printf("Client %s:REQUEST wait -> IP DISTRIBUTE\n", inet_ntoa(tmp_client->id));
				}
				else if(code == '\x03'){
					tmp_client->status = 3;
					tmp_client->ttlcounter = ttl;
					tmp_client->ttl = ttl;
					s_buf[0] = '\x04';
					s_buf[1] = '\x00';
					sendto(sd, s_buf, 100, 0, (struct sockaddr *)&cl_sock, sizeof(cl_sock));
					printf("Send ACK Code=0\n");
					printf("Client %s:IP DISTRIBUTE -> IP DISTRIBUTE\n", inet_ntoa(tmp_client->id));
				}
				else{
					s_buf[0] = '\x04';
					s_buf[1] = '\x04';
					sendto(sd, s_buf, 100, 0, (struct sockaddr *)&cl_sock, sizeof(cl_sock));
					printf("Invalid Code\n");
					printf("Send ACK Code=4\n");
				}
			}
			else{
				printf("Unknown client sent REQUEST\n");
			}
		}
		else if(type == '\x05'){
			printf("Recieve RELEASE\n");
			if((tmp_client = search_client(&used_head, cl_sock.sin_addr.s_addr)) != NULL){
				if(tmp_client->addr.s_addr == tmp_ip){
					addpair_tail(&free_head, tmp_client->given);
					rmclient(tmp_client);
					printf("Client %s:IP DISTRIBUTE -> EXIT\n", inet_ntoa(tmp_client->id));
					free(tmp_client);
				}
				else{
					printf("Client %s was not distributed that IP adress\n", inet_ntoa(tmp_client->id));
				}
			}
			else{
				printf("Unknown client sent RELEASE\n");
			}
		}
		else{
			printf("Invalid type\n");
		}
	}
	return 0;
}
