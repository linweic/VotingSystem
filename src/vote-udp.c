#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define BUF_SIZE 256
#define ID "4"

void error(const char *);
void check_recv(int, char *);

int main (int argc, char *argv[])
{
	int status, sockfd, send_len, recv_len;
	struct addrinfo hints;
	struct addrinfo *servinfo, *res;
	struct sockaddr_storage incoming_addr;
	socklen_t addr_size;
	char buffer[BUF_SIZE],msg[] = ID;

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
	hints.ai_socktype = SOCK_DGRAM; //UDP socket type
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

	printf("Please enter the candidate name and your voter id:\n");
	if(fgets(buffer, 256, stdin)<0){
		error("scanf::");
	}
	buffer[strlen(buffer)-1] = '\0';
	//send an identifier first
	send_len = sendto(sockfd, msg, strlen(msg), 0,
		res->ai_addr, res->ai_addrlen);
	if(send_len < 0){
		error("send identifier ::");
	}
	printf("identifier \"%s\" has been sent, length: %d\n", msg, send_len );

	//send real message
	send_len = sendto(sockfd, buffer, strlen(buffer), 0, 
		res->ai_addr, res->ai_addrlen);
	if(send_len < 0){
		error("send.");
	}
	printf("\"%s\" has been sent, length: %d\n", buffer, send_len);

	//receieve response from server
	addr_size = sizeof(incoming_addr);
	recv_len = recvfrom(sockfd, buffer, BUF_SIZE, 0, (struct sockaddr *) &incoming_addr, &addr_size);
	check_recv(recv_len, buffer);
	printf("\"%s\" receieved from server, length: %d\n", buffer, recv_len);

	return 0;
}

void error(const char *msg){
	perror(msg);
	exit(1);
}
void check_recv(int len, char *buffer){
	if(len <0){
		error("receieve ::");
	}
	else if(len == 0){
		printf("The remote socket has closed connection on you.\n");
	}
	buffer[len] = '\0';
}