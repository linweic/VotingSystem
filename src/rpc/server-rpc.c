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
