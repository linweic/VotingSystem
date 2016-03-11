.SUFFIXES:
.SUFFIXES: .c .o
BIN = ./bin
CLNT = change-password
SRVR = server_svc
CFLAGS = -g -Wall

SRVR_OBJ = server-rpc.o vvote_xdr.o vvote_svc.o
CLNT_OBJ = change-password-rpc.o vvote_xdr.o vvote_clnt.o

default: $(CLNT) $(SRVR)

$(CLNT): $(CLNT_OBJ)
	gcc -o $(CLNT) $(CLNT_OBJ)

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

