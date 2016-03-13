.SUFFIXES:
.SUFFIXES: .c .o
BIN = ./bin
CHANGEPW = change-password-rpc
ADDVOTER = add-voter-rpc
LISTCAN = list-candidates-rpc
VIEWRES = view-result-rpc
VOTECOUNT = vote-count-rpc
VOTE = vote-rpc
VOTEZERO = vote-zero-rpc
CLNT = $(CHANGEPW) $(ADDVOTER) $(LISTCAN) $(VIEWRES) $(VOTECOUNT) $(VOTE) $(VOTEZERO)
SRVR = server_svc
CFLAGS = -g -Wall

SRVR_OBJ = server-rpc.o vvote_xdr.o vvote_svc.o
CHANGEPW_OBJ = change-password-rpc.o vvote_xdr.o vvote_clnt.o
ADDVOTER_OBJ = add-voter-rpc.o vvote_xdr.o vvote_clnt.o
LISTCAN_OBJ = list-candidates-rpc.o vvote_xdr.o vvote_clnt.o
VIEWRES_OBJ = view-result-rpc.o vvote_xdr.o vvote_clnt.o
VOTECOUNT_OBJ = vote-count-rpc.o vvote_xdr.o vvote_clnt.o
VOTE_OBJ = vote-rpc.o vvote_xdr.o vvote_clnt.o
VOTEZERO_OBJ = vote-zero-rpc.o vvote_xdr.o vvote_clnt.o

default: $(CLNT) $(SRVR)

$(CLNT): $(CHANGEPW_OBJ) $(ADDVOTER_OBJ) $(LISTCAN_OBJ) $(VIEWRES_OBJ) $(VOTECOUNT_OBJ) $(VOTE_OBJ) $(VOTEZERO_OBJ)
	gcc -o $(CHANGEPW) $(CHANGEPW_OBJ)
	gcc -o $(ADDVOTER) $(ADDVOTER_OBJ)
	gcc -o $(LISTCAN) $(LISTCAN_OBJ)
	gcc -o $(VIEWRES) $(VIEWRES_OBJ)
	gcc -o $(VOTECOUNT) $(VOTECOUNT_OBJ)
	gcc -o $(VOTE) $(VOTE_OBJ)
	gcc -o $(VOTEZERO) $(VOTEZERO_OBJ)

$(SRVR): $(SRVR_OBJ)
	gcc -o $(SRVR) $(SRVR_OBJ)

vvote_xdr.o: ../vvote_xdr.c
	gcc -c $(CFLAGS) ../vvote_xdr.c

vvote_svc.o: ../vvote_svc.c
	gcc -c $(CFLAGS) ../vvote_svc.c

vvote_clnt.o: ../vvote_clnt.c
	gcc -c $(CFLAGS) ../vvote_clnt.c

server-rpc.o: ../server-rpc.c
	gcc -c $(CFLAGS) ../server-rpc.c

change-password-rpc.o: ../change-password-rpc.c
	gcc -c $(CFLAGS) ../change-password-rpc.c

add-voter-rpc.o: ../add-voter-rpc.c
	gcc -c $(CFLAGS) ../add-voter-rpc.c

list-candidates-rpc.o: ../list-candidates-rpc.c
	gcc -c $(CFLAGS) ../list-candidates-rpc.c

view-result-rpc.o: ../view-result-rpc.c
	gcc -c $(CFLAGS) ../view-result-rpc.c

vote-count-rpc.o: ../vote-count-rpc.c
	gcc -c $(CFLAGS) ../vote-count-rpc.c

vote-rpc.o: ../vote-rpc.c
	gcc -c $(CFLAGS) ../vote-rpc.c

vote-zero-rpc.o: ../vote-zero-rpc.c
	gcc -c $(CFLAGS) ../vote-zero-rpc.c
