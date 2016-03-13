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

char ** zeroize_1_svc(void *ptr, struct svc_req *req){
	static char *result;
	deleVoList(&vhead);
	deleCanList(&chead);
	result = "TRUE";
	return &result;
}

char ** addvoter_1_svc(char ** voterid, struct svc_req *req){
	static char *result;
	int id = atoi(*voterid);
	if(id == 0) {
		result = "ERROR: invalid voter id.";
		return &result;
	} 	
	if(search_voter(&vhead, id)!= NULL){
		result = "EXISTS";
		return &result;	
	}
	Voter *voter = (Voter*) malloc(sizeof(Voter));
	if(voter == NULL) {
		result = "ERROR: Failed to allocate memory to voter pointer. ";
		return &result;
	}
	voter->id = id;
	voter->voted = 0;
	voter->next = vhead;
	vhead = voter;
	//for debugging
	printvoters(vhead);

	result = "OK";
	return &result;
}

char ** votefor_1_svc(Votefor_Param *votefor_param, struct svc_req *req){
	static char *result;
	printf("[DEBUG]candidate name is: %s\n", votefor_param->candi_name);
	printf("[DEBUG]voter id is: %d\n", votefor_param->voterid);
	Voter *voter = search_voter(&vhead, votefor_param->voterid);
	if(voter == NULL){//voter does not exist
		result = "NOTAVOTER";
		return &result;	
	}
	else if((voter->voted) == 1){//voter already voted
		result = "ALREADYVOTED";
		return &result;		
	}
	
	Candidate *ptr = chead;
	while(ptr != NULL){
		int cmp = strcmp(votefor_param->candi_name, ptr->name);
		if(cmp == 0){
			//candidate's name already exists, increment votes
			(ptr->votes) += 1;
			//mark voter as voted
			voter->voted = 1;
			bubble(&chead, ptr);
			//for debugging
			printcandidates(chead);
			result = "EXISTS";
			return &result;
		}
		ptr = ptr->next;
	}
	//candiate does not exist, add new candidate, set votes as 1
	ptr = (Candidate*) malloc(sizeof(Candidate));
	//int len = strlen(votefor_param->candi_name) + 1;//length of candidate name including null terminator
	//ptr->name = (char*)malloc(len * sizeof(char));
	ptr->name = (char*)malloc(BUF_SIZE);
	strcpy(ptr->name, votefor_param->candi_name);
	ptr->votes = 1;
	ptr->next = chead;
	chead = ptr;
	//mark voter as voted
	voter->voted = 1;
	//for debugging
	printcandidates(chead);	
	result = "NEW";
	return &result;
}

char ** listcandidates_1_svc(void *ptr, struct svc_req *req){
	//printf("entering list candidates call.\n");
	static char *result = NULL;
	if(result == NULL) result = (char*)malloc(BUF_SIZE * sizeof(char));
	//response[0] = '\0';
	result[0] = '\0';
	Candidate *cur = chead;
	while(cur!=NULL){
		strcat(result, cur->name);
		strcat(result, "\n");
		cur = cur -> next;
	}
	printf("%s", result);
	return &result;	
}

char ** votecount_1_svc(char **name, struct svc_req *req){
	Candidate *cur = chead;
	static char *result = NULL;
	if(result == NULL) result = (char*)malloc(BUF_SIZE * sizeof(char));
	result[0] = '\0';	
	while(cur != NULL){
		int cmp = strcmp(cur->name, *name);
		if(cmp == 0){
			sprintf(result, "%d", cur->votes);
			return &result;
		}
		cur = cur -> next;
	}
	sprintf(result, "%d", -1);
	return &result;
}

char ** viewresult_1_svc(Credential *cred, struct svc_req *req){
	//*response = '\0';
	static char *result = NULL;
	if(result == NULL) result = (char*)malloc(BUF_SIZE * sizeof(char));
	result[0] = '\0';
	if(check_credential(cred, username, pwd) == 0){
		//find winner or tie
		find_max(&chead, result);
		strcat(result,"\n");
		Candidate *cur = chead;
		//append all candidates and their votes to response
		while(cur != NULL){
			strcat(result, cur->name);
			strcat(result, "\t");
			char *votes_num = (char*)malloc(10);
			sprintf(votes_num, "%d\n", cur->votes);
			strcat(result, votes_num);
			free(votes_num);
			cur = cur->next;
		}
	}
	else strcpy(result, "UNAUTHORIZED");
	return &result;	
}

void printvoters(Voter *head){
	while(head != NULL){
		printf("Voter%d, voted:%u\n", head->id, head->voted);
		head = head->next;
	}
}

void printcandidates(Candidate *head){
	while(head != NULL){
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

u_int check_credential(Credential *cred, char *username, char *password){
	if(cred->username == NULL || cred->password == NULL){
		return 1;
	}	
	int name_cmp = strcmp(username, cred->username);
	int pwd_cmp = strcmp(password, cred->password);
	if(name_cmp == 0 && pwd_cmp == 0) return 0; //credentials match
	else return 1;//credential does not match
}

//This method makes sure candidates with most votes stay at the end of the list
// and does not care about the ordering of others
void bubble(Candidate **head_ref, Candidate *candidate){
	//If candidate already at the end of the list, do nothing
	if(candidate->next == NULL) return;
	//Candidate* prev = candidate;
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
	//strncpy(can1->name, can2->name, sizeof(can2->name));
	strcpy(can1->name, can2->name);
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
	//int max = tail->votes;
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
