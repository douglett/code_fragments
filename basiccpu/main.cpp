#include <iostream>
#include "basiccpu.h"
#include "basicp.h"

using namespace std;
using namespace bc;

void testparse();


int main() {
	printf("start\n");
	testparse();
}

void testparse() {
	printf("load and parse:\n");
	//CPU cpu;
	// vector<uint16_t> prog;
	
	// test parseprog
	basic::fparse("example.bas");
	basic::showprog(basic::prog);
}