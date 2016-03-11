struct Candidate {
   String  name;
   int   votes;
   Candidate *next;
};

struct Voter{
	int id;
	short voted;
	Voter *next;
};

program VOTINGSYS {
	version VOTINGSYS_V1 {
		string changepassword(string, string, string) = 1;
	} = 1;
} = 0x2fffffff;