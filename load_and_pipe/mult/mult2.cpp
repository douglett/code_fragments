#include "pipe/pipe.h"

using namespace std;


void PIPE_dothread() {
	pipe::initcheck();
}

void PIPE_step() {
	if (!pipe::ready())  return;
	int a = 0;
	*pipe::in >> a;
	*pipe::infl = 0;
	*pipe::out << a*2;
	*pipe::outfl = 1;
}