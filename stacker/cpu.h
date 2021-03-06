#pragma once

#include <stdint.h>
#include <string>
#include <vector>

enum CPU_OPCODE {
	OP_NOOP = 0,
	OP_ADD,
	OP_SUB,
	OP_MUL,
	OP_DIV
};

class CPU {
public:
	static const int MEM_SIZE = 0x10000;
	uint16_t mem[MEM_SIZE] = { 0 };
	uint16_t top = MEM_SIZE - 1;
	std::vector<uint16_t> frames;
	CPU();
	void        reset();
	void        loadprog(const std::vector<uint16_t>& prog);
	uint16_t    op(uint16_t o, uint16_t a, uint16_t b);
	uint16_t    stack(uint16_t val);
	uint16_t    stack(const std::string& s);
	uint16_t    get(uint16_t p);
	std::string getstr(uint16_t p);
};