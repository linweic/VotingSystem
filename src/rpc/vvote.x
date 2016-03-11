const PORTNUM  = "6106";
const MAX_PENDING = 5;
const BUF_SIZE = 256;

struct Candidate {
   string  name<BUF_SIZE>;
   int   votes;
   Candidate *next;
};

struct Voter{
	int id;
	unsigned int voted;
	Voter *next;
};

struct Credential {
	string username<BUF_SIZE>;
	string password<BUF_SIZE>;
	string newpassword<BUF_SIZE>;
};

program VOTINGSYS {
	version VOTINGSYS_V1 {
		string changepassword(Credential) = 1;
	} = 1;
} = 0x2fffffff;