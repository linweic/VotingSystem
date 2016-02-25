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
void send_resp(int *, char *, int);
void printvoters(struct Voter*);
void printcandidates(struct Candidate*);
void deleCanList(struct Candidate**);
void deleVoList(struct Voter**);
struct Voter *search_voter(struct Voter **, int);
short check_credential(char *, char *, char *);
void bubble(struct Candidate **, struct Candidate *);
void exchange(struct Candidate *, struct Candidate *, char*);

//functions
char *changepassword(char *, char *, char*);
char *zeroize(struct Candidate**, struct Voter**);
char *addvoter(char *, struct Voter**);
char *votefor(char *, struct Voter **, struct Candidate **);
void listcandidates(struct Candidate *, char *);
void votecount(struct Candidate **, char *, char *);
char *viewresult(char *, struct Candidate **, char*, char*);

//int voter_count;
//struct Candidate *candi_list[BUF_SIZE]; //a list of struct Candidate pointers
										//referring to candidate variables 
//struct Voter *voter_list[BUF_SIZE];

int main (int argc, char *argv[])
{
	int status, sockfd, new_sockfd, recv_len, shutdown = 0;
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

	while(shutdown == 0){
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
			case '2':
				printf("invoke zeroize\n");
				strcpy(response, zeroize(&chead, &vhead));
				//voter_count = 0;
				send_resp(&new_sockfd, response, strlen(response));

				break;
			case '3':
				printf("invoke addvoter\n");
				recv_len = recv(new_sockfd, buffer, BUF_SIZE, 0);
				check_recv(recv_len, buffer);
				printf("\"%s\" receieved, length: %d.\n", buffer, recv_len);
				strcpy(response, addvoter(buffer, &vhead));
				send_resp(&new_sockfd, response, strlen(response));
				printvoters(vhead);
				break;
			case '4':
				printf("invoke votefor\n");
				recv_len = recv(new_sockfd, buffer, BUF_SIZE, 0);
				check_recv(recv_len, buffer);
				printf("\"%s\" receieved, length: %d.\n", buffer, recv_len);
				strcpy(response, votefor(buffer, &vhead, &chead));
				send_resp(&new_sockfd, response, strlen(response));
				printvoters(vhead);
				printcandidates(chead);
				break;
			case '5':
				printf("invoke listcandidates\n");
				listcandidates(chead, response);
				send_resp(&new_sockfd, response, strlen(response));
								
				break;
			case '6':
				printf("invoke votecount\n");
				recv_len = recv(new_sockfd, buffer, BUF_SIZE, 0);
				check_recv(recv_len, buffer);
				printf("\"%s\" receieved, length: %d.\n", buffer, recv_len);
				votecount(&chead, buffer, response);
				send_resp(&new_sockfd, response, strlen(response));
				break;
			case '7':
				printf("invoke viewresult\n");
				recv_len = recv(new_sockfd, buffer, BUF_SIZE, 0);
				check_recv(recv_len, buffer);
				printf("\"%s\" receieved, length: %d.\n", buffer, recv_len);
				strcpy(response, viewresult(buffer, &chead, username, pwd));
				send_resp(&new_sockfd, response, strlen(response));			
				shutdown = 1;
				puts("Server shutting down.");
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
	deleCanList(&chead);
	deleVoList(&vhead);
	//free(buffer);
	//free(response);
	//free(username);
	//free(pwd);
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

//This method makes sure candidates with most votes stay at the front of the list
// and does not care about the ordering of others
void bubble(struct Candidate **head_ref, struct Candidate *candidate){
	struct Candidate* cur = *head_ref;
	if(cur == candidate) return;
	int cmp = (cur->votes) - (candidate->votes);
	if(cmp > 0){
		//candidate does not have maximum number of votes, ignore
		return;
	}
	else if(cmp == 0){
		//candidate has a tie with highest candidates
		//exchange candidate with the node next to the group of winners
		while(cur->next != candidate){
			if((cur->next->votes) < (candidate->votes)){
				//when we find the first node(cur->next) with votes fewer than candidate
				struct Candidate* prev = cur->next;
				while(prev->next != candidate){
					//find the node previous to candidate
					prev = prev->next;
				}
				exchange(cur, prev, "NOTHEAD");
				break;
			}
			else cur = cur -> next;
		}
	}
	else{
		//candidate is the single winner, simply change with the head node
		struct Candidate* prev = cur;
		while(prev->next != candidate){
			//find the node previous to candidate
			prev = prev->next;
		}
		exchange(*head_ref, prev, "HEAD");
	}

}
void exchange(struct Candidate *cur, struct Candidate *prev, char* signal){
	if(strcmp(signal, "NOTHEAD")){
		//exchange cur->next with prev->next
		printf("exchange %s with %s\n", cur->next->name, prev->next->name);
		struct Candidate *ptr = cur->next;
		struct Candidate *candi = prev->next;
		if(cur->next == prev){
			ptr->next = candi->next;
			candi->next = ptr;
			cur->next = candi;
		}
		else{
			struct Candidate *ptr_next = ptr->next;
			ptr->next = candi->next;
			candi->next = ptr_next;
			cur->next = candi;
			prev->next = ptr;
		}
	}
	else{
		//exchange cur with prev->next
		printf("exchange %s with %s\n", cur->name, prev->next->name);
		struct Candidate *candi = prev->next;
		if(cur == prev){
			cur->next = candi->next;
			candi->next = cur;
		}
		else{
			struct Candidate *cur_next = cur->next;
			cur->next = candi->next;
			candi->next = cur_next;
			prev->next = cur;
		}
	}
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
char *find_max(struct Candidate** head_ref){
	struct Candidate *cur = *head_ref;
	if(cur == NULL) return NULL;
	int count = 1;
	int max = cur->votes;
	char *list = cur->name, *role = "winner:";
	while(cur->next != NULL){
		if((cur->next->votes) == max){
			strcat(list, " ");
			strcat(list, cur->next->name);
			count++;
			cur = cur->next;
		}
		else break;
	}
	if(count > 1){
		role = "tie:";
		strcat(role, list);
	}
	else{
		strcat(role, list);
	}
	return role;
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
char *viewresult(char *buffer, struct Candidate** head_ref, char* username, char* password){
	char *result = (char *)malloc(BUF_SIZE);
	if(check_credential(buffer, username, password) == 0){
		strcpy(result, find_max(head_ref));
		strcat(result, "\n");
		struct Candidate* cur = *head_ref;
	    while(cur != NULL){
			char *line = (char *)malloc(BUF_SIZE);
	        sprintf(line, "%s\t%d\n", cur->name, cur->votes);
	        strcat(result,line);
	        cur = cur->next;
			free(line);
	    }
		return result;	
	}
	else return "UNAUTHORIZED";
}
