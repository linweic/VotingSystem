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
#define BUF_SIZE 256

//helper methods
void check_recv(int, char *);
void check_send(int);
void send_resp(int *, char *, int);
//functions
char *changepassword(char *, char *, char*);
void zeroize();
void addvoter(int voterid);
void votefor(char *, int);
void listcandidates();
void votecount(char *);
void viewresult(char *, char *);

int main (int argc, char *argv[])
{
	int status, sockfd, new_sockfd, recv_len;
	struct addrinfo hints;
	struct addrinfo *servinfo, *res;
	struct sockaddr_storage incoming_addr;
	socklen_t addr_size;
	char buffer[BUF_SIZE], response[BUF_SIZE];

	char username[BUF_SIZE], pwd[BUF_SIZE];
	//set up username and password
	if(argc == 1){
		strcpy(username,"cis505");
		strcpy(pwd, "project2");
	}
	else if (argc == 3){
		strcpy(username, argv[1]);
		strcpy(pwd, argv[2]);
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

		/*first receieve the identifier*/
		recv_len = recv(new_sockfd, buffer, 1, 0);
		check_recv(recv_len, buffer);
		printf("\"%c\" receieved, length: %d\n", buffer[0], recv_len);
		
		switch(buffer[0]){
			case '1':
				printf("invoke change-password method\n");
				recv_len =recv(new_sockfd, buffer, BUF_SIZE, 0);
				check_recv(recv_len, buffer);
				printf("\"%s\" receieved, length: %d.\n", buffer, recv_len);
				strcpy(response, changepassword(buffer, username, pwd));
				send_resp(&new_sockfd, response, strlen(response));
				printf("current password is \"%s\"\n", pwd);
				break;
				//send(new_sockfd, "OK", strlen("OK"), 0);
			case '2':
				printf("invoke zeroize\n");

				break;
			case '3':
				printf("invoke addvoter\n");

				break;
			case '4':
				printf("invoke votefor\n");

				break;
			case '5':
				printf("invoke listcandidates\n");
				
				break;
			case '6':
				printf("invoke votecount\n");

				break;
			case '7':
				printf("invoke viewresult\n");

				break;
			default:
				printf("illegal identifier.\n");
				send(new_sockfd, "ILLEGAL", strlen("ILLEGAL"), 0);
				break;
		}

		close(new_sockfd);
	}
	close(sockfd);
	return 0;
}

void check_recv(int len, char *buffer){
	if(len <0){
		perror("receieve ::");
		exit(1);
	}
	else if(len == 0){
		printf("The remote socket has closed connection on you.\n");
	}
	buffer[len] = '\0';
}
void check_send(int len){
	if(len < 0){
		perror("send::");
		exit(1);
	}
}
void send_resp(int *sockfd, char *response, int len){
	int send_len = send( *sockfd, response, len, 0);
	check_send(send_len);
	printf("\"%s\" has been sent to client, length: %d\n", response, send_len);
}

char *changepassword(char *buffer, char *username, char *password){
	char delim[] = " ";
	char * token;
	//parse username
	token = strtok(buffer, delim);
	if(token == NULL) return "FALSE";
	//compare username
	printf("[DEBUG]token_1 is \"%s\"\n", token);
	int usr_cmp = strcmp(username, token);
	//parse password
	token = strtok(NULL, delim);
	if(token == NULL) return "FALSE";
	//compare password
	printf("[DEBUG]token_2 is \"%s\"\n", token);
	int pwd_cmp = strcmp(password, token);
	if(usr_cmp == 0 && pwd_cmp == 0){
		//client provides a matching pair of username and password
		//so update password
		token = strtok(NULL, delim);
		if(token == NULL) return "FALSE: Please provide a new password.";
		printf("[DEBUG]token_3 is \"%s\"\n", token);
		strcpy(password, token);
		return "OK";
	}
	else{
		return "FALSE: your username or password is not correct.";
	}
}
