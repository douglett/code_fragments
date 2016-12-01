#include <cassert>
#include "pipe.h"

using namespace std;


// PIPE
void pipeinit(void* in_, void* out_, int* infl_, int* outfl_) {
	using namespace pipe;
	in   = (stringstream*) in_;
	out  = (stringstream*) out_;
	infl  = infl_;
	outfl = outfl_;
}
// void PIPE_dothread();  // overwrite
// void PIPE_step();      // overwrite


namespace pipe {
	std::stringstream  *in = NULL,  *out = NULL;
	int  *infl = NULL,  *outfl = NULL;

	void  initcheck() {
		assert( in != NULL && out != NULL && infl != NULL && outfl != NULL );
	}

	int ready() {
		initcheck();
		return ( *infl > 0 && *outfl == 0 );
	}
} // end pipe