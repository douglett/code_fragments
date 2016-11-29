#pragma once

#include <iostream>
#include <sstream>

extern "C" {
	void PIPE_init(void* in, void* out, int* inf, int* outf);
	void PIPE_dothread();
	void PIPE_step();
}

namespace pipe {
	extern  std::stringstream  *in, *out;
	extern  int   *infl, *outfl;
	void  initcheck();
	int   ready();
} // end pipe