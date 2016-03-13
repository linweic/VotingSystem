#include "vvote.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    Credential *cred;
    CLIENT *cl;
    char **result, *username, *password;
	if(argc != 4){ 
        fprintf(stderr, "Argument mis-match.\n");
        exit(1);
    }   
	username = argv[2];
	password = argv[3];
    cl = clnt_create(argv[1], VOTINGSYS, VOTINGSYS_V1, "tcp");
	if (cl == NULL) {
        printf("error: could not connect to server.\n");
        return 1;
    }   
    printf("Client connects to server successfully.\n");
	
	cred = (Credential*) malloc(sizeof(Credential));
	int name_len = strlen(username)+1;
	cred->username = (char*)malloc(name_len * sizeof(char));
	strncpy(cred->username, username, name_len);
	cred->username[name_len-1] = '\0';
	int pwd_len = strlen(password)+1;
	cred->password = (char*)malloc(pwd_len * sizeof(char));
	strncpy(cred->password, password, pwd_len);
	cred->password[pwd_len-1] = '\0';

	result = viewresult_1(cred, cl);
	if (result == NULL) {
        clnt_perror(cl,argv[1]);
        exit(1);
    }
    printf("%s\n", *result);
    free(cred->username);
    free(cred->password);
    free(cred);
	return 0;
}
