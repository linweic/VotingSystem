/*  Server side of the voting system using UDP */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define PORTNUM "6106"
#define MAX_PENDING 5
#define BUF_SIZE 256

int main (int argc, char *argv[])
{
	int status, sockfd, recv_len;
	struct addrinfo hints;
	struct addrinfo *servinfo, *res;
	struct sockaddr_storage incoming_addr;
	socklen_t addr_size;
	char buffer[BUF_SIZE];

	char *username, *pwd;
	//set up username and password
	if(argc == 1){
		username = "cis505";
		pwd = "project2";
	}
	else if (argc == 3){
		username = argv[1];
		pwd = argv[2];
	}
	else{
		fprintf(stderr,"Argument mis-match.");
		exit(1);
	}

	//reset and fill in hints
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC; //don't care ipv4 or ipv6
	hints.ai_socktype = SOCK_DGRAM; //UDP socket type
	hints.ai_flags = AI_PASSIVE; //wildcard IP address
	hints.ai_protocol = 0; //any protocol

	if ((status = getaddrinfo(NULL, PORTNUM, &hints, &servinfo))!= 0){
		perror(gai_strerror(status));
		exit(1);
	}

	for(res = servinfo; res != NULL; res = res->ai_next){
		if((sockfd = socket(PF_INET, res->ai_socktype, res->ai_protocol))<0){
			perror("simplex-talk: socket");
			exit(1);
		}
		if(bind(sockfd, res->ai_addr, res->ai_addrlen) < 0){
			close(sockfd);			
			perror("simplex-talk: bind");
			continue;
		}
		break; //when the bind succeeds for the first time, exit the loop
	}
	if(res == NULL){
		fprintf(stderr, "server: failed to bind.\n");
		exit(1);
	}
	//freeaddrinfo(servinfo); //finish checking the linked list returned from getaddrinfo()
	printf("Socket binded to local address and port number.\n");

	while(1){
		printf("Waiting for requests from clients...\n");
		addr_size = sizeof(incoming_addr);
		recv_len = recvfrom(sockfd, buffer, BUF_SIZE, 0, (struct sockaddr *) &incoming_addr, &addr_size);
		if(recv_len <0){
			perror("error reading from socket");
			exit(1);
		}
		else if(recv_len == 0){
			printf("The remote socket has closed connection on you.\n");
		}
		buffer[recv_len] = '\0';
		printf("\"%s\" receieved, length: %d\n", buffer, recv_len);
		
	}
	close(sockfd);
	return 0;
}