#include <iostream>
#include <string>
#include "../pipe.h"

using namespace std;


extern "C" {
	void    PIPE_in  (PIPE_t);
	PIPE_t  PIPE_out ();
	int     PIPE_sig ();
}

int buflen = 0;
static char spipe[PIPE_LEN] = "hello world";
PIPE_t mpipe = { PIPE_LEN, spipe };


// take input and do something
void  PIPE_in(PIPE_t in) {
	snprintf(spipe, PIPE_LEN, "recieved: %s", in.c);
	buflen = 1;
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
