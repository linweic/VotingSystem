#include "vvote.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]){
	char **result;//, **name;
	CLIENT *cl;
	if(argc != 3){ 
        fprintf(stderr, "Argument mis-match. \n");
        exit(1);
    }   
	//*name = argv[2];
	cl = clnt_create(argv[1], VOTINGSYS, VOTINGSYS_V1, "tcp");  
    if(cl == NULL){
        printf("error: could not connect to server.\n");
        return 1;
    }   
    printf("Client connects to server successfully.\n");
	result = votecount_1(&argv[2], cl);
	if(result == NULL){
        clnt_perror(cl, argv[1]);
        exit(1);
    }   
    printf("%s\n", *result);
    return 0;
}
