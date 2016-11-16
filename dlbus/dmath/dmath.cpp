#include <iostream>
#include <string>
#include "../pipe.h"

using namespace std;


extern "C" {
	void    PIPE_in  (PIPE_t);
	PIPE_t  PIPE_out ();
	int     PIPE_sig ();
}

int state = 0;
int buflen = 0;
static char spipe[PIPE_LEN] = "hello world";
PIPE_t mpipe = { PIPE_LEN, spipe };


void do_double(PIPE_t in) {
	if (in.l < sizeof(int))  return;
	int n = *((int*) in.c);
	n *= 2;
	char* c = (char*) &n;
	for (int i = 0; i < sizeof(int); i++)
		spipe[i] = c[i];
	spipe[sizeof(int)] = '\0';
	buflen = 1;
}


// take input and do something
void  PIPE_in(PIPE_t in) {
	string s;
	switch (state) {
	case 1:  do_double(in);  state = 0;  break;
	default:
		s = in.c;
		if (s.substr(0, 8) == "[double]") {
			state = 1;
		}
	}
}

// pipe output
PIPE_t  PIPE_out() {
	buflen--;
	return mpipe;
}

// get status of pipe
int  PIPE_sig() {
	return buflen;
}
