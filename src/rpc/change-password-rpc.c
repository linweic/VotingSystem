#include "vvote.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void error(const char *);

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
    puts("Client connects to server successfully.");
    //create credential object
    cred = (Credential*) malloc(sizeof(Credential));

    puts("Please enter username.");
    if(fgets(input, BUF_SIZE, stdin)<0){
    	error("scanf::");
    }
    input[strlen(input)-1] = '\0';
    strcpy(cred->username, input);

    puts("Please enter your old password.");
    if(fgets(input, BUF_SIZE, stdin)<0){
    	error("scanf::");
    }
    input[strlen(input)-1] = '\0';
    strcpy(cred->password, input);

    puts("Please enter your new password.");
    if(fgets(input, BUF_SIZE, stdin)<0){
    	error("scanf::");
    }
    input[strlen(input) - 1] = '\0';
    strcpy(cred->newpassword, input);

    // call remote procedure 
    result = changepassword_1(*cred, cl);
    if (result == NULL) {
        printf("error: RPC failed!\n");
        exit(1);
    }
    printf("%s\n", *result);

    return 0;
}

void error(const char *msg){
	perror(msg);
	exit(1);
}