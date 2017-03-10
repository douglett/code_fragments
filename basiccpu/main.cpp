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
	// test parseprog
	if (basic::fparse("example.bas"))  return 1;
	printf("=====\n");
	printf("running:\n");
	CPU cpu;
	bc::load(cpu, basic::prog);
	while (!bc::step(cpu))  ;
	printf("[PC %d]  [A %d]  [B %d]\n", cpu.PC, cpu.A, cpu.B);
	return 0;
}