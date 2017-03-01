#include <iostream>
#include <vector>
#include "basiccpu.h"
#include "basicparse.h"

using namespace std;
using namespace bc;

void test1();
void testparse();


int main() {
	printf("start\n");
	// test1();
	testparse();
}


void test1() {
	CPU cpu;
	reset(cpu);
	vector<uint16_t> prog;
	prog.insert(prog.end(), { 
		imerge(OP_SET, ADR_A, ADR_NWD), 10,
		imerge(OP_ADD, ADR_A, ADR_NWD), 2,
		imerge(OP_JSR, ADR_NWD, 0), 20
	});
	memcpy(cpu.ram, &prog[0], prog.size() * sizeof(uint16_t));
	while (step(cpu) == 0)  ;
	printf("cycles: %d\n", cpu.CYC);
	printf("A: %d \n", cpu.A);
}

void testparse() {
	CPU cpu;
	reset(cpu);
	vector<uint16_t> prog;
	if (parse::parse("example.bas", prog))  return;
	memcpy(cpu.ram, &prog[0], prog.size() * sizeof(uint16_t));

	parse::showprog(prog);
	while (step(cpu) == 0)  ;
}