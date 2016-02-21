#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define BUF_SIZE 256;

void error(const char *msg){
	perror(msg);
	exit(1);
}

int main (int argc, char *argv[])
{
	int status, sockfd;
	struct addrinfo hints;
	struct addrinfo *servinfo, *res;
	char buffer[BUF_SIZE],username[BUF_SIZE], pwd[BUF_SIZE], new_pwd[BUF_SIZE];

	if(argc != 3){
		fprintf(stderr, "Argument mis-match.\n");
		exit(1);
	}

	char *serv_addr = argv[1];
	char *port_num = argv[2];
	printf("server address is: %s\n", serv_addr);
	printf("port number is: %s\n", port_num);

	//reset and fill in hints
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC; //don't care ipv4 or ipv6
	hints.ai_socktype = SOCK_STREAM; //TCP socket type
	hints.ai_flags = AI_PASSIVE; //wildcard IP address
	hint.ai_protocol = 0; //any protocol

	if ((status = getaddrinfo(serv_addr, port_num, &hints, &servinfo))!= 0){
		error(gai_strerror(status));
	}

	for(res = servinfo; res != null; res = res->ai_next){
		if((sockfd = socket(PF_INET, res->ai_socktype, res->ai_protocol))<0){
			error("simplex-talk: socket");
		}
		if(connect(sockfd, res->ai_addr, res->ai_addrlen) < 0){
			close(sockfd);			
			perror("simplex-talk: connect");
			continue;
		}
		break; //when the connect succeeds for the first time, exit the loop
	}
	freeaddrinfo(servinfo); //finish checking the linked list returned from getaddrinfo()
	
	if(res == null){
		fprintf(stderr, "client: failed to connect.\n");
		exit(1);
	}
	printf("Client connects to local address and port number.\n");

	printf("Please enter username, your old password and the new password:\n");
	while(scanf("%s", buffer)){
		buffer[BUF_SIZE-1] = '\0';
		int sent_len = send(sockfd, buffer, sizeof(buffer), 0);
		if(sent_len < 0){
			error("message emission failed.");
		}
		printf("%d bytes of string \"%s\" has been sent.\n", sent_len, buffer);
	}
	
	
}