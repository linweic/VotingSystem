#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define BUF_SIZE 256

void error(const char *msg){
	perror(msg);
	exit(1);
}
void sendpkg(int *sfd, char *buf, struct addrinfo *dist){
	if(scanf("%s", buf)<0){
		error("Failed to read input.");
	}
	printf("read input successfully.\n");
	int sent_len = sendto(*sfd, buf, BUF_SIZE, 0, 
		dist->ai_addr, dist->ai_addrlen);
	if(sent_len < 0){
		error("Failed to send package.");
	}
	printf("%d bytes of \"%s\" have been sent", sent_len, buf);
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
	hints.ai_protocol = 0; //any protocol

	if ((status = getaddrinfo(serv_addr, port_num, &hints, &servinfo))!= 0){
		error(gai_strerror(status));
	}

	for(res = servinfo; res != NULL; res = res->ai_next){
		if((sockfd = socket(PF_INET, res->ai_socktype, res->ai_protocol))<0){
			perror("simplex-talk: socket");
			continue;
		}
		break; 	//when the socket is created
				//successfully for the first time, exit the loop
	}	
	if(res == NULL){
		fprintf(stderr, "client: failed to create socket.\n");
		exit(1);
	}
	freeaddrinfo(servinfo); //finish checking the linked list returned from getaddrinfo()
	printf("Socket created.\n");

	printf("Please enter username:\n");
	sendpkg(&sockfd, buffer, res);
	printf("Please enter current password:\n");
	sendpkg(&sockfd, buffer, res);
	printf("Please enter the new password:\n");
	sendpkg(&sockfd, buffer, res);

	return 0;
}