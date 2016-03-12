#include "vvote.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char ** changepassword_1_svc(Credential *credential, struct svc_req *req){
	static char *result; 
	//parse username
	if(credential->username == NULL){
		result = "FALSE";
		return (&result);
	}
	printf("[DEBUG]credential username is \"%s\"\n", credential->username);
	//compare username
	int usr_cmp = strcmp(credential->username, username);
	//parse password
	if(credential->password == NULL){
		result = "FALSE";
		return (&result);
	}
	printf("[DEBUG]credential password is \"%s\"\n", credential->password);
	//compare password
	int pwd_cmp = strcmp(credential->password, pwd);
	//parse new password
	if(credential->newpassword == NULL){
		result = "FALSE";
		return (&result);
	}
	printf("[DEBUG]credential new password is \"%s\"\n", credential->newpassword);
	
	if(usr_cmp == 0 && pwd_cmp == 0){
		//credential matches
		strcpy(pwd, credential->newpassword);
		result = "TRUE";
		return (&result);
	}
	else{
		result = "FALSE";
		return (&result);
	}
}

char ** zeroize_1_svc(void *, struct svc_req *req){
	static char *result;
	deleVoList(&vhead);
	deleCanList(&chead);
	result = "TRUE";
	return &result;
}

char ** addvoter_1_svc(char ** voterid, struct svc_req *req){
	
}

char ** votefor_1_svc(Votefor_Param *votefor_param, struct svc_req *req){
}

char ** listcandidates_1_svc(void *, struct svc_req *req){
}

char ** votecount_1_svc(char **name, struct svc_req *req){
}

char ** viewresult_1_svc(Credential *cred, struct svc_req *req){
}

void printvoters(Voter *head){
	while(!head == NULL){
		printf("Voter%d, voted:%u\n", head->id, head->voted);
		head = head->next;
	}
}

void printcandidates(Candidate *head){
	while(!head == NULL){
		printf("Candidate: %s, votes:%d\n", head->name, head->votes);
		head = head->next;
	}
}

void deleCanList(Candidate **head_ref){
	Candidate *current = *head_ref;
	Candidate *next;
	while(current != NULL){
		next = current->next;
		free(current);
		current = next;
	}
	*head_ref = current;
}

void deleVoList(Voter **head_ref){
	Voter *current = *head_ref;
	Voter *next;
	while(current != NULL){
		next = current->next;
		free(current);
		current = next;
	}
	*head_ref = current;
}

Voter *search_voter(Voter **head_ref, int id){
	Voter* cur = *head_ref;
	while(cur != NULL){
		if((cur->id) == id) return cur;
		cur = cur->next;
	}
	return NULL;
}

//This method makes sure candidates with most votes stay at the end of the list
// and does not care about the ordering of others
void bubble(Candidate **head_ref, Candidate *candidate){
	//If candidate already at the end of the list, do nothing
	if(candidate->next == NULL) return;
	Candidate* prev = candidate;
	Candidate* cur = candidate->next;
	//go through the node from candidate to end, 
	//if cur has fewer votes than candidate, check its next
	//if cur has fewer votes and comes to the end of the list, exchange contents in cur and candidate
	//if cur has fewer votes and its next has the same votes as candidate, exchange contents in cur and candidate
	//if cur has fewer votes and its next has more votes, do nothing
	//if cur has same no fewer votes than candidate, do nothing
	while(cur->votes < candidate->votes){
		Candidate* cur_next = cur->next;
		if(cur_next == NULL || (cur_next->votes == candidate->votes)){
			exchangeContent(cur, candidate);
			break;
		}
		cur = cur->next;
	}   
}

void exchangeContent(Candidate *can1, Candidate *can2){
	char *tmp_name = (char*) malloc(BUF_SIZE);
	strcpy(tmp_name, can1->name);
	int tmp_votes = can1->votes;
	strncpy(can1->name, can2->name, sizeof(can2->name));
	can1->votes = can2->votes;
	strncpy(can2->name, tmp_name, BUF_SIZE);
	can2->votes = tmp_votes;
	free(tmp_name);
}

void find_max(Candidate** head_ref, char* response){
	Candidate *tail = getTail(*head_ref);
	if(tail == NULL) {
		strcpy(response, "NULL");
		return;
	}
	Candidate *cur = *head_ref;
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
Candidate* getTail(Candidate* head){
	Candidate* cur = head;
	if(cur == NULL) return NULL;
	while(cur->next != NULL){
		cur = cur->next;
	}
	return cur;
}
