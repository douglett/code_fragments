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
	uint16_t val_t;

	static string strlower(const string& str) {
		string s = str;
		for (auto& c : s)
			c = tolower(c);
		return s;
	}

	int load(const string& fname, CPU& cpu) {
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
		static const int DISPLAY = 0;
		// display
		if (DISPLAY) {
			for (auto& t : tok)
				printf("[%s]  ", t.c_str());
			printf("\n");
		}
		// setup
		string t;
		char o=0, a=0, b=0;
		uint16_t aa, bb;
		prog = { };
		// parse
		for (int i = 0; i < tok.size(); i++) {
			if (tok[i] == "if") {
				// arg1
				a = addr(tok[++i]),  aa = val_t;
				assert(a != ADR_NIL);
				// comparison
				t = tok[++i];
				if      (t == "=")   o = OP_IFE;
				else if (t == "==")  o = OP_IFE;
				else if (t == "!=")  o = OP_IFN;
				// arg2
				b = addr(tok[++i]),  bb = val_t;
				assert(b != ADR_NIL);
				// decoration
				assert(tok[++i] == "then");
			} 
			else if (tok[i] == "end") {
				assert(tok[++i] == "if");
				continue;
			}
			else if (tok[i] == "let") {
				// arg1
				a = addr(tok[++i]),  aa = val_t;
				assert(b != ADR_NIL);
				// equals
				assert(tok[++i] == "=");
				o = OP_SET;
				// arg2
				b = addr(tok[++i]),  bb = val_t;
				assert(a != ADR_NIL);
			} 
			else if (tok[i] == "add") {
				o = OP_ADD;
				// arg1
				a = addr(tok[++i]),  aa = val_t;
				assert(a != ADR_NIL);
				// arg2
				b = addr(tok[++i]),  bb = val_t;
				assert(b != ADR_NIL);
			}
			else if (tok[i] == "print") {
				o = OP_PRNT;
				a = addr(tok[++i]),  aa = val_t;
				assert(a != ADR_NIL);
				b = 0;
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
				string out = strfmt("%x %x %x", o, a, b);
				if (a == ADR_NWD || a == ADRW_NWD || b == ADR_NWD || b == ADRW_NWD) {
					out += "  (";
					if (a == ADR_NWD || a == ADRW_NWD)  out += strfmt("%d  ",aa);
					if (b == ADR_NWD || b == ADRW_NWD)  out += strfmt("%d",bb);
					out += ")";
				}
				cout << out << endl;
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
			// int l = progln(&cpu.ram[i], s);
			// if (l == 0)  break;
			// i += l;
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
		// printf("%-6s %-3s %-3s\n", so.c_str(), sa.c_str(), sb.c_str());
		// printf("%s %s %s\n", so.c_str(), sa.c_str(), sb.c_str());
		str = strfmt("%-6s %-3s %-3s", so.c_str(), sa.c_str(), sb.c_str());
		return  (i + 1);
	}

	static char addr(const string& t) {
		static const regex REG_NUMBER("[0-9]+");
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