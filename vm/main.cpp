#include <iostream>
#include <stdlib.h>

using namespace std;


namespace vm {

	const int MEM_SIZE = 1024;
	const int STACK_SIZE = 32;

	enum INSTR {
		RET,
		PUSH,			// memory
		POP,
		POKE,
		PEEK,
		POKEIN,
		PEEKIN,
		ADD,			// maths
		SUB,
		MUL,
		DIV,
		MOD,
		CMP,			// compare
		JMP,			// prog flow
		IFJMP,
		IFNJMP,
		PRINT,			// display
		PRINT_NUM
	};
	
	int mem[MEM_SIZE] = { 0 };
	int pc = 0;
	int stack[STACK_SIZE] = { 0 };
	int stack_top = 0;


	int reset() {
		mem[0] = 0;
		pc = 0;
		stack[0] = 0;
		stack_top = 0;
		return 0;
	}


	int load(const int* rom, int len) {
		if (rom == NULL) {
			cerr << "failed to load NULL rom" << endl;
			return 1;
		}
		// do load
		reset();
		for (int i = 0; i < len; i++)
			mem[i] = rom[i];
		return 0;
	}


	int vmprint(int addr) {
		cout << "> ";
		for (int i = 0; char(mem[addr+i]) != '\0'; i++)
			cout << char(mem[addr+i]);
		cout << endl; // flush
		return 0;
	}

	
	int run() {
		while (true) {
			switch (mem[pc]) {
			case RET:
				return stack[stack_top];
			case PUSH:
				stack_top++;
				if (stack_top > STACK_SIZE) {
					cerr << "stack overflow" << endl;
					return 1;
				}
				stack[stack_top] = mem[pc+1];
				pc += 2;
				// cout << "push ";
				// for (int i = 0; i <= stack_top; i++)
				// 	cout << stack[i] << " ";
				// cout << endl;
				break;
			case POP:
				stack_top--;
				if (stack_top < 0) {
					cerr << "stack underflow" << endl;
					return 1;
				}
				break;
			case POKE:
				mem[ mem[pc+1] ] = stack[stack_top];
				stack_top--;
				pc += 2;
				// cout << "pop " << mem[pc+1] << " " << stack[stack_top+1] << endl;
				break;
			case PEEK:
				stack_top++;
				if (stack_top > STACK_SIZE) {
					cerr << "stack overflow" << endl;
					return 1;
				}
				stack[stack_top] = mem[ mem[pc+1] ];
				pc += 2;
				break;
			case POKEIN:
				mem[ mem[ mem[pc+1] ] ] = stack[stack_top];
				// cout << "pokein " << mem[ mem[pc+1] ] << " " << stack[stack_top] << endl;
				stack_top--;
				pc += 2;
				break;
			case PEEKIN:
				stack_top++;
				if (stack_top > STACK_SIZE) {
					cerr << "stack overflow" << endl;
					return 1;
				}
				stack[stack_top] = mem[ mem[ mem[pc+1] ] ];
				// cout << "peekin " << mem[ mem[pc+1] ] << " " << stack[stack_top] << endl;
				pc += 2;
				break;
			case ADD:
				stack[stack_top-1] += stack[stack_top];
				stack_top--;
				pc += 1;
				// cout << "add " << stack[stack_top] << endl;
				break;
			case SUB:
				stack[stack_top-1] -= stack[stack_top];
				stack_top--;
				pc += 1;
				break;
			case MUL:
				stack[stack_top-1] *= stack[stack_top];
				stack_top--;
				pc += 1;
				break;
			case DIV:
				stack[stack_top-1] /= stack[stack_top];
				stack_top--;
				pc += 1;
				break;
			case MOD:
				stack[stack_top-1] %= stack[stack_top];
				stack_top--;
				pc += 1;
				break;
			case CMP:
				// cout << "cmp " << stack[stack_top] << " " << stack[stack_top-1];
				if (stack[stack_top] > stack[stack_top-1])
					stack[stack_top-1] = 1;
				else if (stack[stack_top] < stack[stack_top-1])
					stack[stack_top-1] = -1;
				else
					stack[stack_top-1] = 0;
				// cout << " " << stack[stack_top-1] << endl;
				stack_top--;
				pc += 1;
				break;
			case IFJMP:
				if (stack[stack_top] == 0) 
					pc = mem[pc+1];
				else
					pc += 2;
				stack_top--;
				break;
			case PRINT:
				vmprint(mem[pc+1]);
				pc += 2;
				break;
			case PRINT_NUM:
				cout << "> " << mem[ mem[pc+1] ] << endl;
				pc += 2;
				break;
			default:
				cerr << "unknown opcode" << endl;
				return 1;
			} // end switch
		} // end while
	}

} // end vm


int setstr(const char* str, int* addr) {
	int i;
	for (i = 0; str[i] != '\0'; i++)
		addr[i] = str[i];
	addr[i] = '\0';
	return 0;
}


void showrom() {
	cout << hex;
	for (int i = 0; i < vm::MEM_SIZE; i++) {
		cout << vm::mem[i] << " ";
		if (i % 8 == 7)
			cout << endl;
	}
}


// random tests
int loadprog1() {
	using namespace vm;
	int prog[MEM_SIZE] = {
		PRINT, 900,			// test print
		PUSH, 10,			// do some adding
		PUSH, 15,
		ADD,
		POKE, 800,			// print the result
		PRINT_NUM, 800,
		PUSH, 'a',			// edit the ROM for fun
		POKE, 901,
		PRINT, 900,
		PUSH, 9,			// test comparison
		PUSH, 10,
		CMP,
		POKE, 800,
		PRINT_NUM, 800,
		RET 				// end
	};
	// set memory
	setstr( "hello world lol", prog+900 );

	vm::load( prog, sizeof(prog)/sizeof(int) );
	return 0;
}


// count test
int loadprog2() {
	using namespace vm;
	int prog[MEM_SIZE] = {
		PUSH, 10,			// setup
		POKE, 800,
		PUSH, 20,
		POKE, 801,
		PRINT_NUM, 800, 	// start loop (8)
		PEEK, 800,			// increment by 1
		PUSH, 1,
		ADD,
		POKE, 800,
		PEEK, 801,			// compare
		PEEK, 800,
		CMP,
		PUSH, -1,			// check for less-than
		CMP,
		IFJMP, 8,
		RET
	};

	vm::load( prog, sizeof(prog)/sizeof(int) );
	return 0;
}


// construct an alphabet
int loadprog3() {
	using namespace vm;
	int prog[MEM_SIZE] = {
		PUSH, 'a',			// setup
		POKE, 900,
		PUSH, 900,
		POKE, 800,
		PEEKIN, 800,		// [8] poke memory at address in 800 (a)
		PUSH, 1,			// add 1 char
		ADD,
		PEEK, 800,			// increment poke counter
		PUSH, 1,
		ADD,
		POKE, 800,
		POKEIN, 800,		// add letter to new string
		PUSH, 925,			// compare with alphabet length
		PEEK, 800,
		CMP,
		PUSH, -1,
		CMP,
		IFJMP, 8,
		PEEK, 800,			// increment poke counter
		PUSH, 1,
		ADD,
		POKE, 800,
		PUSH, '\0',			// add null terminater
		POKEIN, 800,
		PRINT, 900,			// print alphabet!
		RET
	};

	vm::load( prog, sizeof(prog)/sizeof(int) );
	return 0;
}


// int main() {
// 	loadprog3();
// 	// showrom();
// 	int result = vm::run();
// 	cout << "exit: " << result << endl;
// }
