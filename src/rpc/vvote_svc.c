/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include "vvote.h"
#include <stdio.h>
#include <stdlib.h>
#include <rpc/pmap_clnt.h>
#include <string.h>
#include <memory.h>
#include <sys/socket.h>
#include <netinet/in.h>

#ifndef SIG_PF
#define SIG_PF void(*)(int)
#endif

static void
votingsys_1(struct svc_req *rqstp, register SVCXPRT *transp)
{
	union {
		Credential changepassword_1_arg;
		char *addvoter_1_arg;
		Votefor_Param votefor_1_arg;
		char *votecount_1_arg;
		Credential viewresult_1_arg;
	} argument;
	char *result;
	xdrproc_t _xdr_argument, _xdr_result;
	char *(*local)(char *, struct svc_req *);

	switch (rqstp->rq_proc) {
	case NULLPROC:
		(void) svc_sendreply (transp, (xdrproc_t) xdr_void, (char *)NULL);
		return;

	case changepassword:
		_xdr_argument = (xdrproc_t) xdr_Credential;
		_xdr_result = (xdrproc_t) xdr_wrapstring;
		local = (char *(*)(char *, struct svc_req *)) changepassword_1_svc;
		break;

	case zeroize:
		_xdr_argument = (xdrproc_t) xdr_void;
		_xdr_result = (xdrproc_t) xdr_wrapstring;
		local = (char *(*)(char *, struct svc_req *)) zeroize_1_svc;
		break;

	case addvoter:
		_xdr_argument = (xdrproc_t) xdr_wrapstring;
		_xdr_result = (xdrproc_t) xdr_wrapstring;
		local = (char *(*)(char *, struct svc_req *)) addvoter_1_svc;
		break;

	case votefor:
		_xdr_argument = (xdrproc_t) xdr_Votefor_Param;
		_xdr_result = (xdrproc_t) xdr_wrapstring;
		local = (char *(*)(char *, struct svc_req *)) votefor_1_svc;
		break;

	case listcandidates:
		_xdr_argument = (xdrproc_t) xdr_void;
		_xdr_result = (xdrproc_t) xdr_wrapstring;
		local = (char *(*)(char *, struct svc_req *)) listcandidates_1_svc;
		break;

	case votecount:
		_xdr_argument = (xdrproc_t) xdr_wrapstring;
		_xdr_result = (xdrproc_t) xdr_wrapstring;
		local = (char *(*)(char *, struct svc_req *)) votecount_1_svc;
		break;

	case viewresult:
		_xdr_argument = (xdrproc_t) xdr_Credential;
		_xdr_result = (xdrproc_t) xdr_wrapstring;
		local = (char *(*)(char *, struct svc_req *)) viewresult_1_svc;
		break;

	default:
		svcerr_noproc (transp);
		return;
	}
	memset ((char *)&argument, 0, sizeof (argument));
	if (!svc_getargs (transp, (xdrproc_t) _xdr_argument, (caddr_t) &argument)) {
		svcerr_decode (transp);
		return;
	}
	result = (*local)((char *)&argument, rqstp);
	if (result != NULL && !svc_sendreply(transp, (xdrproc_t) _xdr_result, result)) {
		svcerr_systemerr (transp);
	}

	if (!svc_freeargs (transp, (xdrproc_t) _xdr_argument, (caddr_t) &argument)) {
		fprintf (stderr, "%s", "unable to free arguments");
		exit (1);
	}
	/*shutdown the server*/
	if(shut_down == 1){
		deleCanList(&chead);
		deleVoList(&vhead);
		free(long_result);
		//svc_destroy(transp);
		printf("server shutting down...\n");
		svc_exit();
	}
	return;
}

/*intialize global variables*/
Candidate *chead = NULL;
Voter *vhead = NULL;
int shut_down = 0;
char buffer[BUF_SIZE] = "";
char response[BUF_SIZE] = "";
char username[BUF_SIZE] = "";
char pwd[BUF_SIZE] = "";
char *long_result = NULL;

int
main (int argc, char **argv)
{
	/*reading command line arguments*/
	if(argc == 1){
		strcpy(username,"cis505");
		strcpy(pwd, "project2");
	}
	else if (argc == 3){
		strcpy(username, argv[1]);
		strcpy(pwd, argv[2]);
	}
	else{
		fprintf(stderr,"Argument mis-match.");
		exit(1);
	}

	register SVCXPRT *transp;

	pmap_unset (VOTINGSYS, VOTINGSYS_V1);

	transp = svcudp_create(RPC_ANYSOCK);
	if (transp == NULL) {
		fprintf (stderr, "%s", "cannot create udp service.");
		exit(1);
	}
	if (!svc_register(transp, VOTINGSYS, VOTINGSYS_V1, votingsys_1, IPPROTO_UDP)) {
		fprintf (stderr, "%s", "unable to register (VOTINGSYS, VOTINGSYS_V1, udp).");
		exit(1);
	}

	transp = svctcp_create(RPC_ANYSOCK, 0, 0);
	if (transp == NULL) {
		fprintf (stderr, "%s", "cannot create tcp service.");
		exit(1);
	}
	if (!svc_register(transp, VOTINGSYS, VOTINGSYS_V1, votingsys_1, IPPROTO_TCP)) {
		fprintf (stderr, "%s", "unable to register (VOTINGSYS, VOTINGSYS_V1, tcp).");
		exit(1);
	}

	svc_run ();
	fprintf (stderr, "%s", "svc_run returned");
	exit (1);
	/* NOTREACHED */
}
