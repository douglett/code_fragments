#include <iostream>
// #include <vector>
#include "cpu.h"

using namespace std;


// globals
CPU cpu;


void test_stack() {
	auto a = cpu.stack(10);
	printf("pos a: %d\n", a);
	auto b = cpu.stack("hello world");
	printf("pos b: %d\n", b);
	// auto c = cpu.stack({ 1, 2, 3 });
	// printf("pos c: %d\n", c);
	auto d = cpu.stack(123);
	printf("pos d: %d\n", d);
	printf("val b: \"%s\"\n", cpu.getstr(b).c_str());
	cpu.reset();
}

void test_math() {
	auto a = cpu.stack(10), b = cpu.stack(20);
	cpu.op(OP_ADD, a, b);
	printf("a: %d\n", cpu.mem[a]);
	cpu.op(OP_MUL, a, b);
	printf("a: %d\n", cpu.mem[a]);
	cpu.reset();
}


int main() {
	cout << "CPU test" << endl;
	test_stack();
	// test_math();
}