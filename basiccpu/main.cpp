#include <iostream>
#include "basiccpu.h"
#include "basicp.h"

using namespace std;
using namespace bc;

int testparse();


int main() {
	printf("start\n");
	testparse();
}

int testparse() {
	printf("load and parse:\n");
	//CPU cpu;
	// vector<uint16_t> prog;
	
	// test parseprog
	if (basic::fparse("example.bas"))
		return 1;
	basic::showprog(basic::prog);
	return 0;
}