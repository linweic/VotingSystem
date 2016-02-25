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

struct Candidate {
   char  name[BUF_SIZE];
   int   votes;
};

struct Voter{
	int id;
	short voted; //0 means not voted yet, 1 mean already voted
};

//helper methods
void check_recv(int, char *);
void check_send(int);
void send_resp(int *, char *, int);
void printvoters(struct Voter**);
void printcandidates(struct Candidate**);
struct Voter *search_voter(struct Voter **, int id);

//functions
char *changepassword(char *, char *, char*);
char *zeroize(struct Voter **, struct Candidate **);
char *addvoter(char *, struct Voter **, int *, struct Voter **);
char *votefor(char *, struct Voter **, struct Candidate **);
char *listcandidates();
char *votecount(char *);
char *viewresult(char *, char *);

int voter_count;
struct Candidate *candi_list[BUF_SIZE]; //a list of struct Candidate pointers
										//referring to candidate variables 
struct Voter *voter_list[BUF_SIZE];

int main (int argc, char *argv[])
{
	int status, sockfd, new_sockfd, recv_len;
	struct addrinfo hints;
	struct addrinfo *servinfo, *res;
	struct sockaddr_storage incoming_addr;
	socklen_t addr_size;
	char buffer[BUF_SIZE], response[BUF_SIZE];
	char username[BUF_SIZE], pwd[BUF_SIZE];

	//set up default username and password
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
				//recv_len = recv(new_sockfd, buffer, BUF_SIZE, 0);
				//check_recv(recv_len, buffer);
				//printf("\"%s\" receieved, length: %d.\n", buffer, recv_len);
				strcpy(response, zeroize(voter_list, candi_list));
				voter_count = 0;
				send_resp(&new_sockfd, response, strlen(response));

				break;
			case '3':
				printf("invoke addvoter\n");
				recv_len = recv(new_sockfd, buffer, BUF_SIZE, 0);
				check_recv(recv_len, buffer);
				printf("\"%s\" receieved, length: %d.\n", buffer, recv_len);
				strcpy(response, addvoter(buffer, voter_list, &voter_count, voter_list+voter_count));
				send_resp(&new_sockfd, response, strlen(response));
				printvoters(voter_list);
				break;
			case '4':
				printf("invoke votefor\n");
				recv_len = recv(new_sockfd, buffer, BUF_SIZE, 0);
				check_recv(recv_len, buffer);
				printf("\"%s\" receieved, length: %d.\n", buffer, recv_len);
				strcpy(response, votefor(buffer, voter_list, candi_list));
				send_resp(&new_sockfd, response, strlen(response));
				printvoters(voter_list);
				printcandidates(candi_list);
				break;
			case '5':
				printf("invoke listcandidates\n");
				recv_len = recv(new_sockfd, buffer, BUF_SIZE, 0);
				check_recv(recv_len, buffer);
				printf("\"%s\" receieved, length: %d.\n", buffer, recv_len);
				//strcpy(response,      );
				send_resp(&new_sockfd, response, strlen(response));
								
				break;
			case '6':
				printf("invoke votecount\n");
				recv_len = recv(new_sockfd, buffer, BUF_SIZE, 0);
				check_recv(recv_len, buffer);
				printf("\"%s\" receieved, length: %d.\n", buffer, recv_len);
				//strcpy(response,      );
				send_resp(&new_sockfd, response, strlen(response));
				
				break;
			case '7':
				printf("invoke viewresult\n");
				recv_len = recv(new_sockfd, buffer, BUF_SIZE, 0);
				check_recv(recv_len, buffer);
				printf("\"%s\" receieved, length: %d.\n", buffer, recv_len);
				//strcpy(response,      );
				send_resp(&new_sockfd, response, strlen(response));
				
				break;
			default:
				printf("illegal identifier.\n");
				strcpy(response, "illegal identifier");
				send_resp(&new_sockfd, response, strlen(response));
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
void printvoters(struct Voter **vlist){
	struct Voter **vptr;
	for(vptr = vlist; *vptr; vptr++){
		printf("Voter%d, voted:%d\n", (*vptr)->id, (*vptr)->voted);
	}
}
void printcandidates(struct Candidate **clist){
	struct Candidate **cptr;
	for(cptr = clist; *cptr; cptr++){
		printf("Candidate: %s, votes:%d\n", (*cptr)->name, (*cptr)->votes);
	}
}
struct Voter *search_voter(struct Voter **vlist, int id){
	struct Voter **vptr;
	for(vptr = vlist; *vptr; vptr++){
		if(id == (*vptr)->id) return *vptr;
	}
	return NULL;
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

char *zeroize(struct Voter **vlist, struct Candidate **clist){
	//clear voteid array
	struct Voter **vptr;
	for(vptr = vlist; *vptr; vptr++){
		*vptr = NULL;
	}
	//clear candidate array
	struct Candidate **cptr;
	for(cptr = clist; *cptr; cptr++){
		*cptr = NULL;
	}
	return "TRUE";
}
/**
*	char* buffer: pointer to the string sent from client
*	struct Voter **vlist: pointer to the voter_list array
*	int* count: the address of the variable that stores the number of voters
*	struct Voter **insert: the pointer pointing to the first NULL pointer in the pointer array
**/
char *addvoter(char *buffer, struct Voter **vlist, int *count, struct Voter **insert){
	if((*count) > BUF_SIZE || (*count) == BUF_SIZE){
		return "ERROR: voters overflow.";
	}
	int id = atoi(buffer);
	if(id == 0) return "ERROR: invalid voter id.";

	if(search_voter(vlist, id)!= NULL) return "EXISTS";
	//TO DO: add new voter, set voted as 0
	*insert = (struct Voter*) malloc(sizeof(struct Voter));
	if((*insert) == NULL) return "ERROR: allocate memory to voter pointer failed";
	(*insert) -> id  = id;
	(*insert) -> voted = 0;
	//increment count
	(*count)+=1;
	return "OK";
}

char *votefor(char *buffer, struct Voter **vlist, struct Candidate **clist){
	char delim[] = " ";
	char *token;
	char *name;
	int id;
	//parse candidate name
	token = strtok(buffer, delim);
	if(token == NULL) return "ERROR";
	printf("[DEBUG]token_1 is \"%s\"\n", token);
	strcpy(name, token);

	//parse voterid
	token = strtok(NULL, delim);
	if(token == NULL) return "ERROR";
	printf("[DEBUG]token_2 is \"%s\"\n", token);
	id = atoi(token);
	if(id == 0) return "ERROR: invalid voter id.";

	//check if voter id exists
	struct Voter *voter = search_voter(vlist, id);
	if(voter == NULL) return  "NOTAVOTER";
	else if((voter->voted) == 1) return "ALREADYVOTED";

	//voter exists and have not voted yet
	struct Candidate **ptr;
	for(ptr = clist; *ptr; ptr++){
		int cmp = strcmp(name, (*ptr)->name);
		if(cmp == 0){
			//candidate's name already exists, increment votes
			((*ptr)-> votes) += 1;
			//mark voter as voted
			voter->voted = 1;
			return "EXISTS";
		}
	}
	//TO DO: add new candidate, set votes as 1
	*ptr = (struct Candidate*) malloc(sizeof(struct Candidate));
	strcpy((*ptr)->name, name);
	(*ptr)->votes = 1;

	//mark voter as voted
	voter->voted = 1;

	return "NEW";
}
