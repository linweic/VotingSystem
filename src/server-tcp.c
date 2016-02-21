/*  Server side of the voting system using TCP */

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

int main (int argc, char *argv[])
{
	int status, sockfd, new_sockfd, recv_len;
	struct addrinfo hints;
	struct addrinfo *servinfo, *res;
	struct sockaddr_storage incoming_addr;
	socklen_t addr_size;
	char buffer[256];

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
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC; //don't care ipv4 or ipv6
	hints.ai_socktype = SOCK_STREAM; //TCP socket type
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
	freeaddrinfo(servinfo); //finish checking the linked list returned from getaddrinfo()

	if(res == NULL){
		fprintf(stderr, "server: failed to bind.\n");
		exit(1);
	}
	printf("Socket binded to local address and port number.\n");

	//Server blocks and wait for requests to arrive
	listen(sockfd, MAX_PENDING);
	printf("listening on requests to arrive...\n");

	while(1){
		addr_size = sizeof(incoming_addr);
		if((new_sockfd = accept(sockfd, (struct sockaddr *) &incoming_addr, &addr_size))<0){
			perror("simplex-talk: accept");
			continue;
		}
		printf("request accepted.\n");

		recv_len = recv(new_sockfd, buffer, sizeof(buffer), 0);
		if(recv_len <0){
			perror("error reading from socket");
			exit(1);
		}
		else if(recv_len == 0){
			printf("The remote socket has closed connection on you.\n");
		}
		//do stuff


		close(new_sockfd);
	}
	close(sockfd);
	return 0;
}