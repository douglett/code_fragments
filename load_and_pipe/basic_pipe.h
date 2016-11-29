#pragma once

#include <iostream>
#include <sstream>

extern "C" {
	void initpipe(void* in, void* out, int* inf, int* outf);
	void dothread();
	void step();
}

namespace pipe {
	extern  std::stringstream  *in, *out;
	extern  int  *infl, *outfl;
} // end pipe