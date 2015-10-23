#include <iostream>
#include <vector>

using namespace std;


namespace basic {

	// const int MEM_SIZE = 1024;
	const int STACK_SIZE = 32;

	enum INSTR {
		RET,
		DIM,
		PUSH,
		POP,
		LOAD,
		SET,
		ADD,
		PRINT
	};

	vector<vector<int> > prog;
	int pc = 0;
	int stack[STACK_SIZE] = { 0 };
	int stack_top = 0;


	int* getptr(char varname) {
		int dimlen = stack[1];
		// cout << dimlen << " " << char(stack[2]) << " " << char(stack[4]) << endl;
		for (int i = 0; i < dimlen; i++) {
			if (stack[2+i*2] == varname)
				return &stack[3+i*2];
		}
		cerr << "var not found: " << varname << endl;
		return NULL;
	}

	
	int run() {
		int proglen = prog.size();

		while (true) {
			// error checking
			if (pc >= proglen) {
				cerr << "pc out of bounds: " << pc << endl;
				return 1;
			}
			if (prog[pc].size() == 0) {
				pc++;
				continue;
			}

			// auto &progln = prog[pc];

			// switch on opcode
			switch (prog[pc][0]) {
			case RET:
				if (prog[pc].size() >= 1)
					return prog[pc][1];
				return stack[stack_top];
			case DIM:
				if (stack_top != 0) {
					cerr << "DIM must be called at stack top only" << endl;
					return 1;
				}
				// push var len
				stack_top++;
				stack[stack_top] = prog[pc].size() - 1;
				// push vars
				for (int i = 1; i < prog[pc].size(); i++) {
					stack_top += 2;
					stack[stack_top-1] = prog[pc][i];
					stack[stack_top] = 0;
				}
				break;
			case PUSH:
				stack_top++;
				stack[stack_top] = prog[pc][1];
				break;
			case POP:
				stack_top--;
				break;
			case LOAD:
				stack_top++;
				// stack[stack_top] = stack[ prog[pc][1] - 'a' ];
				stack[stack_top] = *getptr( prog[pc][1] );
				break;
			case SET:
				// stack[ prog[pc][1] - 'a' ] = stack[stack_top];
				*getptr( prog[pc][1] ) = stack[stack_top];
				stack_top--;
				break;
			case ADD:
				for (int i = 1; i < prog[pc].size(); i++) {
					stack[stack_top] += prog[pc][i];
				}
				break;
			case PRINT:
				// cout << "> " << stack[ prog[pc][1] - 'a' ] << endl;
				cout << "> " << *getptr( prog[pc][1] ) << endl;
				break;
			default:
				cerr << "unknown opcode: " << prog[pc][0] << endl;
				return 1;
			}

			pc++;
		}
	}

} // end basic


int main() {
	using namespace basic;
	prog = vector<vector<int> > {
		{ DIM, 'a', 'i', 'b' },
		{},
		{ PUSH, 0 },
		{ ADD, 10, 10 },
		{ SET, 'a' },
		{ PRINT, 'a' },
		{},
		{ LOAD, 'a' },
		{ ADD, 5 },
		{ SET, 'b' },
		{ PRINT, 'b' },
		{},
		{ RET, 0 }
	};

	int retval = basic::run();
	cout << "return: " << retval << endl;
}
