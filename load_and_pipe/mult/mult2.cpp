#include "pipe/pipe.h"

using namespace std;


void pipethread() {
	pipe::initcheck();
}

void pipestep() {
	if (!pipe::ready())  return;
	int a = 0;
	*pipe::in >> a;
	*pipe::infl = 0;
	*pipe::out << a*2;
	*pipe::outfl = 1;
}