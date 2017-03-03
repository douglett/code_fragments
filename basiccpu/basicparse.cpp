#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

#include "basicparse.h"
#include "basiccpu.h"

using namespace std;
using namespace bc;


namespace bc {
namespace parse {

	// string to lower case
	static string strlower(const string& str) {
		string s = str;
		for (auto& c : s)
			c = tolower(c);
		return s;
	}
	// static int indexOf(const vector<string>& vs, const string& s) {
	// 	for (int i = 0; i < vs.size(); i++)
	// 		if (vs[i] == s)
	// 			return i;
	// 	return -1;
	// }
	// address string to address
	static uint16_t val_t = 0;
	static char addr(const string& t) {
		// static const regex REG_NUMBER("[0-9]+");
		val_t = 0;
		if      (t == "a")   return ADR_A;
		else if (t == "b")   return ADR_B;
		else if (t == "c")   return ADR_C;
		else if (t == "x")   return ADR_X;
		else if (t == "y")   return ADR_Y;
		else if (t == "z")   return ADR_Z;
		else if (t == "i")   return ADR_I;
		else if (t == "j")   return ADR_J;
		else if (t == "pc")  return ADR_PC;
		else if (t == "sp")  return ADR_SP;
		// else if (regex_match(t, REG_NUMBER))  { val_t = stoi(t);  return ADR_NWD; }
		else if (stringstream(t) >> val_t)  return ADR_NWD;
		return ADR_NIL;
	}
	// file name to token list
	static int load_tok(const string& fname, vector<string>& tok) {
		// load from file
		tok = { };
		string s;
		fstream fs(fname, fstream::in);
		if (!fs.is_open()) {
			fprintf(stderr, "could not open file: %s\n", fname.c_str());
			return 1;
		}
		while (!fs.eof()) {
			fs >> ws;
			if      (fs.peek() == '\'')  { getline(fs, s);  tok.push_back(s); }
			else if (fs >> s)  { tok.push_back( strlower(s) ); }
		}
		fs.close();
		return 0;
	}
	// init program
	static void initprog(vector<uint16_t>& prog) {
		prog = {
			0,  // noop
			imerge(OP_JSR, ADRW_NWD, 0), 6,  // run main
			imerge(OP_SET, ADR_PC, ADR_NWD), 0,  // die
			0  // func_count (5)
		};
		// call_block padding
		while (prog.size() < 0x300)
			prog.push_back(0);
	}

	//--- load to CPU struct ---
	int load_parse(const string& fname, CPU& cpu) {
		reset(cpu);
		vector<string> tok;
		if (load_tok(fname, tok))  return 1;
		
		// display
		for (const auto& t : tok)
			printf("[%s]  ", t.c_str());
		cout << endl;

		// prepare
		vector<uint16_t> prog;
		initprog(prog);
		vector<string> bstack, funclist;
		char o, a, b;
		uint16_t aa, bb;

		// parse loop
		for (int i = 0; i < tok.size(); i++) {
			// guard against globals
			if (bstack.size() == 0)
				if (tok[i] != "func") {
					fprintf(stderr, "expected: func (%d)\n", i);
					return 1;
				}
			// comments
			if (tok[i].size() && tok[i][0] == '\'') 
				continue;
			// function definitions
			if (tok[i] == "func") {
				string fn = tok[++i];
				if (find(funclist.begin(), funclist.end(), fn) != funclist.end()) {
					fprintf(stderr, "func: duplicate function (%s) (%d)\n", fn.c_str(), i);
					return 1;	
				}
				funclist.push_back(fn);
				bstack.push_back("func");
				printf("func %s\n", fn.c_str());
				continue;
			}
			// end function block
			if (tok[i] == "end" && tok[i+1] == "func") {
				i++;
				prog.push_back(imerge(OP_RET, 0, 0));
				continue;
			}
			// set values
			if (tok[i] == "let") {
				o = OP_SET;
				a = addr(tok[++i]);
				aa = val_t;
				string eq = tok[++i];
				b = addr(tok[++i]);
				bb = val_t;
				if (a == ADR_NIL || b == ADR_NIL || eq != "=") {
					fprintf(stderr, "let error: %d\n", i);
					return 1;
				}
				// prog.push_back(imerge(o, a, b));
				// if (a == ADR_NWD || a == ADRW_NWD)  prog.push_back(aa);
				// if (b == ADR_NWD || b == ADRW_NWD)  prog.push_back(bb);
				continue;
			}
			// unknown
			fprintf(stderr, "unknown command: %s\n", tok[i].c_str());
			return 1;
		}

		prog[5] = funclist.size();
		vector<uint16_t> names;
		for (const auto& fn : funclist) {
			for (char c : fn)
				names.push_back(c);
			names.push_back(0);
		}
		prog[5 + funclist.size() + 1] = names.size();
		memcpy(&prog[5 + funclist.size() + 2], &names[0], names.size()*sizeof(uint16_t));

		memcpy(cpu.ram, &prog[0], sizeof(uint16_t) * prog.size());
		return 0;  // OK
	}


	void show(CPU& cpu) {
		printf("begin:\n");
		printf("  %s\n", iline(cpu).c_str());
		cpu.PC += 2;
		printf("  %s\n", iline(cpu).c_str());
		cpu.PC += 2;
		printf("call_block:\n");
		// number of functions
		int func_count = cpu.ram[cpu.PC];
		printf("  func_count: %d\n", func_count);
		cpu.PC += func_count + 1;
		// function names
		int func_name_size = cpu.ram[cpu.PC];
		printf("  func_name_size: %d\n", func_name_size);
		cpu.PC++;
		vector<string> func_names = { "" };
		for (int i = 0; i < func_name_size; i++) {
			char c = cpu.ram[cpu.PC++];
			if (c == 0 && i < func_name_size-1)  func_names.emplace_back();
			else  func_names.back() += c;
		}
		for (const auto& n : func_names)
			printf("  >%s\n", n.c_str());
		printf("-----\n");
		cpu.PC = 1;
	}

} // end parse
} // end bc