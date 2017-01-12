#include <iostream>
#include <vector>

using namespace std;


enum OPCODE {
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
	vector<uint16_t> frames;
	CPU() {
		frames.push_back(top);
	}
	void reset() {
		frames = {};
		top = MEM_SIZE - 1;
	}
/*** opcode ***/
	uint16_t op(int o, int a, int b) {
		switch (o) {
		case OP_NOOP:  break;
		case OP_ADD:   mem[a] += mem[b];  break;
		case OP_SUB:   mem[a] -= mem[b];  break;
		case OP_MUL:   mem[a] *= mem[b];  break;
		case OP_DIV:   mem[a] /= mem[b];  break;
		}
		return 0;
	}
/*** helpers ***/
	uint16_t stack(uint16_t v) {
		mem[top] = v;
		return top--;
	}
	uint16_t stack(const string& s) {
		auto start = top;
		mem[top--] = s.length();
		for (auto c : s)
			mem[top--] = c;
		return start;
	}
	uint16_t get(uint16_t p) {
		return mem[p];
	}
	string getstr(uint16_t p) {
		string s;
		int len = mem[p--];
		for (int i = 0; i < len; i++)
			s.push_back(mem[p - i]);
		return s;
	}
};
CPU cpu;


void stack_test() {
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


int main() {
	cout << "asd" << endl;
	stack_test();
	
	auto a = cpu.stack(10), b = cpu.stack(20);
	cpu.op(OP_ADD, a, b);
	printf("a: %d\n", cpu.mem[a]);
	cpu.op(OP_MUL, a, b);
	printf("a: %d\n", cpu.mem[a]);
}