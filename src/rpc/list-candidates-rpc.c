#include "vvote.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    CLIENT *cl;
    void *ptr = NULL;
    char **result;
    
    if(argc != 2){ 
        fprintf(stderr, "Argument mis-match. \n");
        exit(1);
    }   
    cl = clnt_create(argv[1], VOTINGSYS, VOTINGSYS_V1, "tcp");  
    if(cl == NULL){
        printf("error: could not connect to server.\n");
        return 1;
    }   
    printf("Client connects to server successfully.\n");
	result = listcandidates_1(ptr, cl);
	if(result == NULL){
        clnt_perror(cl, argv[1]);
        exit(1);
    }   
    printf("%s\n", *result);
    return 0;
}
