#include "vvote.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
	char **result, **candi_name, **voter_id;
	CLIENT *cl;
	Votefor_Param *param;
	
	if(argc != 4){
        fprintf(stderr, "Argument mis-match.\n");
        exit(1);
    }
	*candi_name = argv[2];
	*voter_id = argv[3];

	cl = clnt_create(argv[1], VOTINGSYS, VOTINGSYS_V1, "tcp");
    if (cl == NULL) {
        printf("error: could not connect to server.\n");
        return 1;
    }
    printf("Client connects to server successfully.\n");

	//Construct Votefor_Param
	param = (Votefor_Param*) malloc(sizeof(Votefor_Param));
	
	int name_len = strlen(*candi_name)+1;//including null terminator
	param->candi_name = (char*) malloc(name_len * sizeof(char));
	strncpy(param->candi_name, *candi_name, name_len);
	param->candi_name[name_len-1] = '\0';

	param->voterid = atoi(*voter_id);

	//call remote procedure
	result = votefor_1(param, cl);
	if (result == NULL) {
        clnt_perror(cl,argv[1]);
        exit(1);
    }
    printf("%s\n", *result);
	free(param->candi_name);
	free(param);
	return 0;
}
