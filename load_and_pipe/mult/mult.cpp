#include <iostream>
#include <sstream>
#include <cassert>

using namespace std;

extern "C" {
	void initpipe(void* in, void* out, int* inf, int* outf);
	void dothread();
	void step();
}

namespace pipe {
	std::stringstream  *in = NULL,  *out = NULL;
	int  *infl = NULL,  *outfl = NULL;
} // end pipe

using namespace pipe;


static void initcheck() {
	assert( in != NULL && out != NULL && infl != NULL && outfl != NULL );
}


void initpipe(void* in_, void* out_, int* infl_, int* outfl_) {
	in   = (stringstream*) in_;
	out  = (stringstream*) out_;
	infl  = infl_;
	outfl = outfl_;
}

void dothread() {
	// nothing
	if (out != NULL)  *out << "no thread" << endl;
	else  cerr << "no thread" << endl;
}

void step() {
	initcheck();
	if ( !(*infl >= 1 && *outfl == 0) )  return;
	int a = 0;
	*in >> a;
	a *= 2;
	*out << a;
}