/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include "vvote.h"

bool_t
xdr_Candidate (XDR *xdrs, Candidate *objp)
{
	register int32_t *buf;

	 if (!xdr_string (xdrs, &objp->name, BUF_SIZE))
		 return FALSE;
	 if (!xdr_int (xdrs, &objp->votes))
		 return FALSE;
	 if (!xdr_pointer (xdrs, (char **)&objp->next, sizeof (Candidate), (xdrproc_t) xdr_Candidate))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_Voter (XDR *xdrs, Voter *objp)
{
	register int32_t *buf;

	 if (!xdr_int (xdrs, &objp->id))
		 return FALSE;
	 if (!xdr_u_int (xdrs, &objp->voted))
		 return FALSE;
	 if (!xdr_pointer (xdrs, (char **)&objp->next, sizeof (Voter), (xdrproc_t) xdr_Voter))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_Credential (XDR *xdrs, Credential *objp)
{
	register int32_t *buf;

	 if (!xdr_string (xdrs, &objp->username, BUF_SIZE))
		 return FALSE;
	 if (!xdr_string (xdrs, &objp->password, BUF_SIZE))
		 return FALSE;
	 if (!xdr_string (xdrs, &objp->newpassword, BUF_SIZE))
		 return FALSE;
	return TRUE;
}