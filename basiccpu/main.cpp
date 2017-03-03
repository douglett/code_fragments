#include <iostream>
#include <vector>
#include "basiccpu.h"
#include "basicparse.h"

using namespace std;
using namespace bc;

void test1();
void testparse();
void testparse2();


int main() {
	printf("start\n");
	// test1();
	// testparse();
	testparse2();
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
	vector<uint16_t> prog;
	printf("parsing:\n");
	if (parse::load_parse("example.bas", prog))  return;
	parse::showprog(prog);

	printf("running:\n");
	CPU cpu;
	reset(cpu);
	memcpy(cpu.ram, &prog[0], prog.size() * sizeof(uint16_t));
	while (step(cpu) == 0)  ;
}

void testparse2() {
	printf("load and parse:\n");
	CPU cpu;
	if (parse::load_parse("example.bas", cpu))  return;
	// parse::showprog()
	parse::showprog(cpu);
	printf("running:\n");
	while (step(cpu) == 0)  ;
	printf("OK:  CYC %d  PC %d\n", cpu.CYC, cpu.PC);
}