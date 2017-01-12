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
void CPU::loadprog(const vector<uint16_t>& prog) {
	reset();
	for (int i = 0; i < prog.size(); i++)
		mem[i] = prog[i];
}


// opcodes
uint16_t CPU::op(uint16_t o, uint16_t a, uint16_t b) {
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
uint16_t CPU::stack(uint16_t val) {
	mem[top] = val;
	return frames.back() - top--;
}
uint16_t CPU::stack(const string& s) {
	auto start = top;
	mem[top--] = s.length();  // set length
	for (auto c : s)
		mem[top--] = c;  // set values
	return frames.back() - start;  // return frame offset
}
uint16_t CPU::get(uint16_t p) {
	return mem[frames.back() - p];  // val at frame offset
}
string CPU::getstr(uint16_t p) {
	string s;
	int pos = frames.back() - p;  // get frame offset
	int len = mem[pos--];  // string length
	for (int i = 0; i < len; i++)
		s.push_back(mem[pos - i]);  // add each char to string
	return s;
}