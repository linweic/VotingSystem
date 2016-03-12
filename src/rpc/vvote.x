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

struct Votefor_Param{
	string candi_name<BUF_SIZE>;
	int voterid;
};
program VOTINGSYS {
	version VOTINGSYS_V1 {
		string changepassword(Credential) = 1;
		string zeroize(void) = 2;
		string addvoter(string) = 3;
		string votefor(Votefor_Param) = 4;
		string listcandidates(void) = 5;
		string votecount(string) = 6;
		string viewresult(Credential) = 7;
	} = 1;
} = 0x2fffffff;
