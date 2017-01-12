#include "cpu.h"

using namespace std;


// main
CPU::CPU() {
	frames.push_back(top);
}
void CPU::reset() {
	frames = {};
	top = MEM_SIZE - 1;
}


// opcodes
uint16_t CPU::op(int o, int a, int b) {
	switch (o) {
	case OP_NOOP:  break;
	case OP_ADD:   mem[a] += mem[b];  break;
	case OP_SUB:   mem[a] -= mem[b];  break;
	case OP_MUL:   mem[a] *= mem[b];  break;
	case OP_DIV:   mem[a] /= mem[b];  break;
	default:       fprintf(stderr, "CPU::op : unknown opcode : %d\n", o);
	}
	return 0;
}


// helpers
uint16_t CPU::stack(uint16_t v) {
	mem[top] = v;
	return top--;
}
uint16_t CPU::stack(const string& s) {
	auto start = top;
	mem[top--] = s.length();
	for (auto c : s)
		mem[top--] = c;
	return start;
}
uint16_t CPU::get(uint16_t p) {
	return mem[p];
}
string CPU::getstr(uint16_t p) {
	string s;
	int len = mem[p--];
	for (int i = 0; i < len; i++)
		s.push_back(mem[p - i]);
	return s;
}