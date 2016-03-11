#include "vvote.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char ** changepassword_1_svc(Credential credential, struct svc_req *req){
	static char *result; 
	//parse username
	if(credential.username == NULL){
		strcpy(result, "FALSE");
		return (&result);
	}
	printf("[DEBUG]credential username is \"%s\"\n", credential.username);
	//compare username
	int usr_cmp = strcmp(credential.username, username);
	//parse password
	if(credential.password == NULL){
		strcpy(result, "FALSE");
		return (&result);
	}
	printf("[DEBUG]credential password is \"%s\"\n", credential.password);
	//compare password
	int pwd_cmp = strcmp(credential.password, pwd);
	//parse new password
	if(credential.newpassword == NULL){
		strcpy(result, "FALSE");
		return (&result);
	}
	printf("[DEBUG]credential new password is \"%s\"\n", credential.newpassword);
	
	if(usr_cmp == 0 && pwd_cmp == 0){
		//credential matches
		strcpy(pwd, credential.newpassword);
		strcpy(result,"TRUE");
		return (&result);
	}
	else{
		strcpy(result, "FALSE");
		return (&result);
	}
}