#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <cassert>
#include <regex>

#include "basicparse.h"
#include "basiccpu.h"

using namespace std;
using namespace bc;


namespace bc {
namespace parse {

	static char addr   (const string& t);
	static int  progln (const uint16_t* prog, string& str);
	static uint16_t val_t;
	static vector<pair<string, uint16_t>> blockstack;

	static string strlower(const string& str) {
		string s = str;
		for (auto& c : s)
			c = tolower(c);
		return s;
	}

	int load(const string& fname, CPU& cpu) {
		reset(cpu);
		vector<uint16_t> prog;
		int err = load(fname, prog);
		if (err)  return err;
		memcpy(cpu.ram, &prog[0], sizeof(uint16_t) * prog.size());
		return 0;
	}

	int load(const string& fname, vector<uint16_t>& prog) {
		// load from file
		vector<string> tok;
		string s;
		fstream fs(fname, fstream::in);
		while (!fs.eof()) {
			fs >> ws;
			if      (fs.peek() == '\'')  { getline(fs, s);  tok.push_back(s); }
			else if (fs >> s)  { tok.push_back( strlower(s) ); }
		}
		fs.close();
		// parse
		return parse(tok, prog);
	}

	int parse(const vector<string>& tok, vector<uint16_t>& prog) {
		static const int DISPLAY = 1;
		// display
		if (DISPLAY) {
			for (auto& t : tok)
				printf("[%s]  ", t.c_str());
			printf("\n");
		}
		// setup
		char o=0, a=0, b=0;
		uint16_t aa, bb;
		prog = { };
		// parse
		for (int i = 0; i < tok.size(); i++) {
			int start = prog.size();
			if (tok[i] == "if") {
				// arg1
				a = addr(tok[++i]),  aa = val_t;
				// comparison
				string cmp = tok[++i];
				if      (cmp == "=")   o = OP_IFE;
				else if (cmp == "==")  o = OP_IFE;
				else if (cmp == "!=")  o = OP_IFN;
				else    o = OP_NOOP;
				// arg2
				b = addr(tok[++i]),  bb = val_t;
				// decoration
				string sthen = tok[++i];
				// error check
				if (o == OP_NOOP || a == ADR_NIL || b == ADR_NIL || sthen != "then") {
					fprintf(stderr, "parse error: if statement (%d - %d)\n", i-4, i);
					return 1;
				}
				// set up the instruction
				prog.push_back(imerge(o, a, b));
				if (a == ADR_NWD || a == ADRW_NWD)  prog.push_back(aa);
				if (b == ADR_NWD || b == ADRW_NWD)  prog.push_back(bb);
				prog.insert(prog.end(), { 
					imerge(OP_ADD, ADR_PC, ADR_NWD), 2,  // if correct, skip next instruction
					imerge(OP_SET, ADR_PC, ADR_NWD), 0   // set PC to some temp value
				});
				blockstack.push_back({ "if", prog.size()-1 });
				if (DISPLAY)  printf("if...\n");
				continue;
			} 
			else if (tok[i] == "end") {
				string etype = tok[++i];
				if (blockstack.back().first == etype) {
					prog[ blockstack.back().second ] = prog.size();
				}
				// error check
				else {
					fprintf(stderr, "parse error: end statement (%d - %d)\n", i-1, i);
					return 1;
				}
				continue;
			}
			else if (tok[i] == "let") {
				// arg1
				a = addr(tok[++i]),  aa = val_t;
				// equals
				string eq = tok[++i];
				o = OP_SET;
				// arg2
				b = addr(tok[++i]),  bb = val_t;
				// error check
				if (a == ADR_NIL || b == ADR_NIL || eq != "=") {
					fprintf(stderr, "parse error: let statement (%d - %d)\n", i-3, i);
					return 1;
				}
			} 
			else if (tok[i] == "add") {
				o = OP_ADD;
				// arg1
				a = addr(tok[++i]),  aa = val_t;
				// arg2
				b = addr(tok[++i]),  bb = val_t;
				// error check
				if (a == ADR_NIL || b == ADR_NIL) {
					fprintf(stderr, "parse error: add statement (%d - %d)\n", i-2, i);
					return 1;
				}
			}
			else if (tok[i] == "print") {
				o = OP_PRNT;
				a = addr(tok[++i]),  aa = val_t;
				b = 0;
				// error check
				if (a == ADR_NIL) {
					fprintf(stderr, "parse error: print statement (%d - %d)\n", i-1, i);
					return 1;
				}
			}
			else {
				if (tok[i].substr(0, 1) == "'")  ;  // comment - ignore
				else  printf("unknown token:  %s\n", tok[i].c_str());
				continue;
			}
			// add to prog
			prog.push_back(imerge(o, a, b));
			if (a == ADR_NWD || a == ADRW_NWD)  prog.push_back(aa);
			if (b == ADR_NWD || b == ADRW_NWD)  prog.push_back(bb);
			// display
			if (DISPLAY) {
				// raw display
				string out = strfmt("%x %x %x", o, a, b);
				if (a == ADR_NWD || a == ADRW_NWD || b == ADR_NWD || b == ADRW_NWD) {
					out += "  (";
					if (a == ADR_NWD || a == ADRW_NWD)  out += strfmt("%d  ",aa);
					if (b == ADR_NWD || b == ADRW_NWD)  out += strfmt("%d",bb);
					out += ")";
				}
				// compiled line
				string asmb;
				progln(&prog[start], asmb);
				// print
				printf("[%-15s]----[%-10s]\n", out.c_str(), asmb.c_str());
			}
		}
		// return OK
		return 0;
	}

	void showprog(const vector<uint16_t>& prog) {
		string s;
		for (int i = 0; i < prog.size(); ) {
			int l = progln(&prog[i], s);
			if (l == 0)  break;
			i += l;
			printf("%s\n", s.c_str());
		}
	}

	void showprog(const CPU& cpu) {
		string s;
		char o;
		for (int i = 0; i < 0x10000; ) {
			isplit(cpu.ram[i], &o, NULL, NULL);
			if (o == OP_NOOP)  break;
			i += progln(&cpu.ram[i], s);
			printf("%s\n", s.c_str());
		}
	}

	static int progln(const uint16_t* prog, string& str) {
		// init
		assert(prog != NULL);
		char o, a, b;
		uint16_t i=0, in;
		string so, sa, sb;
		str = "";
		// setup
		in = prog[i];
		isplit(in, &o, &a, &b);
		so = inameop(o);
		// get second argument
		if      (a == ADR_NWD )  sa = strfmt( "%d",   prog[++i] );
		else if (a == ADRW_NWD)  sa = strfmt( "[%d]", prog[++i] );
		else    sa = inameaddr(a);
		if      (b == ADR_NWD )  sb = strfmt( "%d",   prog[++i] );
		else if (b == ADRW_NWD)  sb = strfmt( "[%d]", prog[++i] );
		else    sb = inameaddr(b);
		// print
		str = strfmt("%-6s %-3s %-3s", so.c_str(), sa.c_str(), sb.c_str());
		// str = strfmt("%s %s %s", so.c_str(), sa.c_str(), sb.c_str());
		return  (i + 1);
	}

	static char addr(const string& t) {
		static const regex REG_NUMBER("[0-9]+");
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
		else if (regex_match(t, REG_NUMBER))  { val_t = stoi(t);  return ADR_NWD; }
		return ADR_NIL;
	}

} // end parse
} // end bc