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

struct Candidate {
   char  name[BUF_SIZE];
   int   votes;
   struct Candidate *next;
};

struct Voter{
	int id;
	short voted; //0 means not voted yet, 1 mean already voted
	struct Voter *next;
};

//helper methods
void check_recv(int, char *);
void check_send(int);
void send_resp(int *, char *, int, struct sockaddr*, socklen_t*);
void printvoters(struct Voter*);
void printcandidates(struct Candidate*);
void deleCanList(struct Candidate**);
void deleVoList(struct Voter**);
struct Voter *search_voter(struct Voter **, int);
short check_credential(char *, char *, char *);
void bubble(struct Candidate **, struct Candidate *);
void exchangeContent(struct Candidate *, struct Candidate *);
void find_max(struct Candidate**, char* );
struct Candidate* getTail(struct Candidate*);

//functions
char *changepassword(char *, char *, char*);
char *zeroize(struct Candidate**, struct Voter**);
char *addvoter(char *, struct Voter**);
char *votefor(char *, struct Voter **, struct Candidate **);
void listcandidates(struct Candidate *, char *);
void votecount(struct Candidate **, char *, char *);
void viewresult(char *, struct Candidate **, char*, char*, char*);

int main (int argc, char *argv[])
{
	int status, sockfd, recv_len, shutdown = 0;
	struct addrinfo hints;
	struct addrinfo *servinfo, *res;
	struct sockaddr_storage incoming_addr;
	socklen_t addr_size;
	char buffer[BUF_SIZE] = "", response[BUF_SIZE] = "";
	char username[BUF_SIZE] = "", pwd[BUF_SIZE] = "";

	struct Candidate* chead = NULL;
	struct Voter* vhead = NULL;
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
	//freeaddrinfo(servinfo); //finish checking the linked list returned from getaddrinfo()

	if(res == NULL){
		fprintf(stderr, "server: failed to bind.\n");
		exit(1);
	}
	//freeaddrinfo(servinfo); 
	//finish checking the linked list returned from getaddrinfo()
	printf("Socket binded to local address and port number.\n");

	while(shutdown == 0){
		printf("Waiting for requests from clients...\n");
		addr_size = sizeof(incoming_addr);
		recv_len = recvfrom(sockfd, buffer, 1, 0, (struct sockaddr *) &incoming_addr, &addr_size);
		check_recv(recv_len, buffer);
		printf("\"%c\" receieved, length: %d\n", buffer[0], recv_len);

		switch(buffer[0]){
			case '1':
				printf("invoke change-password method\n");
				addr_size = sizeof(incoming_addr);
				recv_len = recvfrom(sockfd, buffer, BUF_SIZE, 0, (struct sockaddr *) &incoming_addr, &addr_size);
				check_recv(recv_len, buffer);
				printf("\"%s\" receieved, length: %d.\n", buffer, recv_len);
				strcpy(response, changepassword(buffer, username, pwd));
				send_resp(&sockfd, response, strlen(response), (struct sockaddr*)&incoming_addr, &addr_size);
				printf("current password is \"%s\"\n", pwd);
				break;
			case '2':
				printf("invoke zeroize\n");
				addr_size = sizeof(incoming_addr);
				strcpy(response, zeroize(&chead, &vhead));
				//voter_count = 0;
				send_resp(&sockfd, response, strlen(response), (struct sockaddr*)&incoming_addr, &addr_size);
				break;
			case '3':
				printf("invoke addvoter\n");
				recv_len = recvfrom(sockfd, buffer, BUF_SIZE, 0, (struct sockaddr *) &incoming_addr, &addr_size);
				check_recv(recv_len, buffer);
				printf("\"%s\" receieved, length: %d.\n", buffer, recv_len);
				strcpy(response, addvoter(buffer, &vhead));
				send_resp(&sockfd, response, strlen(response), (struct sockaddr*)&incoming_addr, &addr_size);
				printvoters(vhead);
				break;
			case '4':
				printf("invoke votefor\n");
				recv_len = recvfrom(sockfd, buffer, BUF_SIZE, 0, (struct sockaddr *) &incoming_addr, &addr_size);
				check_recv(recv_len, buffer);
				printf("\"%s\" receieved, length: %d.\n", buffer, recv_len);
				strcpy(response, votefor(buffer, &vhead, &chead));
				send_resp(&sockfd, response, strlen(response), (struct sockaddr*)&incoming_addr, &addr_size);
				printvoters(vhead);
				printcandidates(chead);
				break;
			case '5':
				printf("invoke listcandidates\n");
				listcandidates(chead, response);
				send_resp(&sockfd, response, strlen(response), (struct sockaddr*)&incoming_addr, &addr_size);			
				break;
			case '6':
				printf("invoke votecount\n");
				recv_len = recvfrom(sockfd, buffer, BUF_SIZE, 0, (struct sockaddr *) &incoming_addr, &addr_size);
				check_recv(recv_len, buffer);
				printf("\"%s\" receieved, length: %d.\n", buffer, recv_len);
				votecount(&chead, buffer, response);
				send_resp(&sockfd, response, strlen(response), (struct sockaddr*)&incoming_addr, &addr_size);
				break;
			case '7':
				printf("invoke viewresult\n");
				recv_len = recvfrom(sockfd, buffer, BUF_SIZE, 0, (struct sockaddr *) &incoming_addr, &addr_size);
				check_recv(recv_len, buffer);
				printf("\"%s\" receieved, length: %d.\n", buffer, recv_len);
				viewresult(buffer, &chead, username, pwd, response);
				send_resp(&sockfd, response, strlen(response), (struct sockaddr*)&incoming_addr, &addr_size);			
				if(strcmp(response,"UNAUTHORIZED") != 0){
					shutdown = 1;
					puts("Server shutting down.");
				}
				break;
			/*
			default:
				printf("illegal identifier.\n");
				strcpy(response, "illegal identifier");
				send_resp(&sockfd, response, strlen(response), res);
				break;
			*/
		}
		
	}
	close(sockfd);
	deleCanList(&chead);
	deleVoList(&vhead);
	freeaddrinfo(servinfo); //finish checking the linked list returned from getadd    rinfo()
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
		perror("send:");
		exit(1);
	}
}
void send_resp(int *sockfd, char *response, int len, struct sockaddr* addr, socklen_t* addrlen){
	int send_len = sendto( *sockfd, response, len, 0,
		 addr, *addrlen);
	check_send(send_len);
	printf("\"%s\" has been sent to client, length: %d\n", response, send_len);
}
void printvoters(struct Voter *head){
	while(head != NULL){
		printf("Voter%d, voted:%d\n", head->id, head->voted);
		head = head -> next;
	}
}
void printcandidates(struct Candidate *head){
	while(head != NULL){
		printf("Candidate: %s, votes:%d\n", head->name, head->votes);
		head = head -> next;
	}
}
void deleCanList(struct Candidate** head_ref){
	struct Candidate* current = *head_ref;
	struct Candidate* next;
	while(current != NULL){
		next = current->next;
		free(current);
		current = next;
	}
	*head_ref = current;
}
void deleVoList(struct Voter** head_ref){
	struct Voter* current = *head_ref;
	struct Voter* next;
	while(current != NULL){
		next = current->next;
		free(current);
		current = next;
	}
	*head_ref = current;
}
struct Voter *search_voter(struct Voter **head_ref, int id){
	struct Voter* cur = *head_ref;
	while(cur != NULL){
		if((cur->id) == id) return cur;
		cur = cur->next;
	}
	return NULL;
}

//This method makes sure candidates with most votes stay at the end of the list
// and does not care about the ordering of others
void bubble(struct Candidate **head_ref, struct Candidate *candidate){
	//If candidate already at the end of the list, do nothing
	if(candidate->next == NULL) return;
	struct Candidate* prev = candidate;
	struct Candidate* cur = candidate->next;
	//go through the node from candidate to end, 
	//if cur has fewer votes than candidate, check its next
	//if cur has fewer votes and comes to the end of the list, exchange contents in cur and candidate
	//if cur has fewer votes and its next has the same votes as candidate, exchange contents in cur and candidate
	//if cur has fewer votes and its next has more votes, do nothing
	//if cur has same no fewer votes than candidate, do nothing
	while(cur->votes < candidate->votes){
		struct Candidate* cur_next = cur->next;
		if(cur_next == NULL || (cur_next->votes == candidate->votes)){
			exchangeContent(cur, candidate);
			break;
		}
		cur = cur->next;
	}   
}
void exchangeContent(struct Candidate *can1, struct Candidate *can2){
	char *tmp_name = (char*) malloc(BUF_SIZE);
	strcpy(tmp_name, can1->name);
	int tmp_votes = can1->votes;
	strncpy(can1->name, can2->name, sizeof(can2->name));
	can1->votes = can2->votes;
	strncpy(can2->name, tmp_name, BUF_SIZE);
	can2->votes = tmp_votes;
	free(tmp_name);
}
	
short check_credential(char *buffer, char *username, char *password){
	//return 0 means credential match, otherwise 1
	char delim[] = " ";
	char * token;
	//parse username
	token = strtok(buffer, delim);
	if(token == NULL) return 1;
	//compare username
	printf("[DEBUG]token_1 is \"%s\"\n", token);
	int usr_cmp = strcmp(username, token);
	//parse password
	token = strtok(NULL, delim);
	if(token == NULL) return 1;
	//compare password
	printf("[DEBUG]token_2 is \"%s\"\n", token);
	int pwd_cmp = strcmp(password, token);
	if(usr_cmp == 0 && pwd_cmp == 0){ return 0;}
	else return 1;
}
void find_max(struct Candidate** head_ref, char* response){
	struct Candidate *tail = getTail(*head_ref);
	if(tail == NULL) return;
	struct Candidate *cur = *head_ref;
	int max = tail->votes;
	//find the starting point of all the winners
	while(cur->votes != tail->votes){
		cur = cur->next;
	}
	if(cur == tail){
		sprintf(response, "Winner: %s", tail->name);
	}
	else {
		sprintf(response, "Tie: %s ", tail->name);
		//assemble all the ties from cur to tail
		while(cur!=tail){
			strcat(response, cur->name);
			strcat(response, " ");
			cur = cur->next;
		}
	}
}
struct Candidate* getTail(struct Candidate* head){
	struct Candidate* cur = head;
	if(cur == NULL) return NULL;
	while(cur->next != NULL){
		cur = cur->next;
	}
	return cur;
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

char *zeroize(struct Candidate** chead_ref, struct Voter** vhead_ref){
	//clear voteid array
	deleVoList(vhead_ref);
	//clear candidate array
	deleCanList(chead_ref);
	return "TRUE";
}
/**
*	char* buffer: pointer to the string sent from client
*	struct Voter **vhead_ref: pointer referrring to the address of the voterlist head
**/
char *addvoter(char *buffer, struct Voter** vhead_ref){
	int id = atoi(buffer);
	if(id == 0) return "ERROR: invalid voter id.";

	if(search_voter(vhead_ref, id)!= NULL) return "EXISTS";
	//add new voter, set voted as 0
	struct Voter* voter = (struct Voter*) malloc(sizeof(struct Voter));
	if(voter == NULL) return "ERROR: allocate memory to voter pointer failed";
	voter -> id  = id;
	voter -> voted = 0;
	voter -> next = *vhead_ref;
	*vhead_ref = voter;
	return "OK";
}

char *votefor(char *buffer, struct Voter **vhead_ref, struct Candidate **chead_ref){
	char delim[] = " ";
	char *token;
	char *name;
	int id;
	//parse candidate name
	token = strtok(buffer, delim);
	if(token == NULL) return "ERROR";
	printf("[DEBUG]token_1 is \"%s\"\n", token);
	name = (char *) malloc(strlen(token)+1);
	strcpy(name, token);

	//parse voterid
	token = strtok(NULL, delim);
	if(token == NULL) return "ERROR";
	printf("[DEBUG]token_2 is \"%s\"\n", token);
	id = atoi(token);
	if(id == 0) return "ERROR: invalid voter id.";

	//check if voter id exists
	struct Voter *voter = search_voter(vhead_ref, id);
	if(voter == NULL) return  "NOTAVOTER";
	else if((voter->voted) == 1) return "ALREADYVOTED";

	//voter exists and have not voted yet
	struct Candidate *ptr = *chead_ref;
	while(ptr != NULL){
		int cmp = strcmp(name, ptr->name);
		if(cmp == 0){
			//candidate's name already exists, increment votes
			(ptr-> votes) += 1;
			//mark voter as voted
			voter->voted = 1;
			bubble(chead_ref, ptr);
			return "EXISTS";
		}
		ptr = ptr -> next;
	}
	//add new candidate, set votes as 1
	ptr = (struct Candidate*) malloc(sizeof(struct Candidate));
	strcpy(ptr->name, name);
	ptr->votes = 1;
	ptr->next = *chead_ref;
	*chead_ref = ptr;
	//mark voter as voted
	voter->voted = 1;

	free(name);
	return "NEW";
}
void listcandidates(struct Candidate *head, char *response){
	*response = '\0';
	while(head != NULL){
		strcat(response, head -> name);
		strcat(response, "\n");
		head = head -> next;
	}
}
void votecount(struct Candidate **head_ref, char *buffer, char *response){
	struct Candidate *cur = *head_ref;
	while(cur != NULL){
		int cmp = strcmp(cur->name, buffer);
		if(cmp == 0){
			//TO DO: convert int to string
			sprintf(response, "%d", cur->votes);
			return;
		}
		cur = cur -> next;
	}
	sprintf(response, "%d", -1);
	return;
}
void viewresult(char *buffer, struct Candidate** head_ref, char* username, char* password, char* response){
	*response = '\0';
	if(check_credential(buffer, username, password) == 0){
		find_max(head_ref, response);
		strcat(response, "\n");
		struct Candidate* cur = *head_ref;
	    while(cur != NULL){
	    	strcat(response, cur->name);
	    	strcat(response, "\t");
	    	char *votes_num = malloc(10);
	    	sprintf(votes_num, "%d\n", cur->votes);
	    	strcat(response, votes_num);
	    	free(votes_num);
	    	cur = cur->next;
	    }
	}
	else strcpy(response,"UNAUTHORIZED");
}
