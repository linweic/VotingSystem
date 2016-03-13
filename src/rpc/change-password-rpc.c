#include "vvote.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    Credential *cred;
    CLIENT *cl;
    char **result;
    char input[BUF_SIZE];

    if(argc != 2){
    	fprintf(stderr, "Argument mis-match.\n");
    	exit(1);
    }

    cl = clnt_create(argv[1], VOTINGSYS, VOTINGSYS_V1, "tcp");
    if (cl == NULL) {
        printf("error: could not connect to server.\n");
        return 1;
    }
    printf("Client connects to server successfully.\n");
    //create credential object
    cred = (Credential*) malloc(sizeof(Credential));

    printf("Please enter username.\n");
    if(fgets(input, BUF_SIZE, stdin)<0){
    	error("scanf::");
    }
    input[strlen(input)-1] = '\0';
	cred->username = (char*)malloc((strlen(input)+1)*sizeof(char));
    strcpy(cred->username, input);
	

    printf("Please enter your old password.\n");
    if(fgets(input, BUF_SIZE, stdin)<0){
    	error("scanf::");
    }
    input[strlen(input)-1] = '\0';
	cred->password = (char*)malloc((strlen(input)+1)*sizeof(char));
    strcpy(cred->password, input);

    printf("Please enter your new password.\n");
    if(fgets(input, BUF_SIZE, stdin)<0){
    	error("scanf::");
    }
    input[strlen(input)-1] = '\0';
	cred->newpassword = (char*)malloc((strlen(input)+1)*sizeof(char));
    strcpy(cred->newpassword, input);

    // call remote procedure 
    result = changepassword_1(cred, cl);
    if (result == NULL) {
        clnt_perror(cl,argv[1]);
        exit(1);
    }
    printf("%s\n", *result);
	free(cred->username);
	free(cred->password);
	free(cred->newpassword);
	free(cred);
    return 0;
}

void error(const char *msg){
	perror(msg);
	exit(1);
}
