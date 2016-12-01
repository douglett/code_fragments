#pragma once

#include <iostream>
#include <sstream>

extern "C" {
	void pipeinit(void* in, void* out, int* inf, int* outf);
	void pipethread();
	void pipestep();
}

namespace pipe {
	extern  std::stringstream  *in, *out;
	extern  int   *infl, *outfl;
	void  initcheck();
	int   ready();
} // end pipe