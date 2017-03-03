#include <iostream>
#include <vector>
#include "basiccpu.h"
#include "basicparse.h"

using namespace std;
using namespace bc;

void testparse();


int main() {
	printf("start\n");
	testparse();
}


void testparse() {
	printf("load and parse:\n");
	CPU cpu;
	if (parse::load_parse("example.bas", cpu))  return;
	
	printf("running:\n");
	while (step(cpu) == 0)  ;
	printf("OK:  CYC %d  PC %d\n", cpu.CYC, cpu.PC);
}