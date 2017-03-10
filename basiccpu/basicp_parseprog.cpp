#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "basicp.h"
#include "basiccpu.h"

using namespace std;
using namespace bc;


namespace basic {

	static uint16_t PC = 0;
	static vector<string> toklist;
	static vector<string> funclist;
	vector<uint16_t> prog;


	static int getnum(const string& s, uint16_t& val) {
		char* endptr;
		val = strtol(s.c_str(), &endptr, 10);
		return (endptr > s.c_str());
	}
	static int clear_comments() {
		while (PC < toklist.size())
			if (toklist[PC][0] == '\'')  PC++;
			else  break;
		return (PC >= toklist.size());
	}
	static const char* ctok() {
		if (PC < toklist.size())  return toklist[PC].c_str();
		return "<EOF>";
	}
	static int tokcmp(const vector<string>& vs) {
		if (PC + vs.size() > toklist.size())  return 0;
		for (int i=0; i<vs.size(); i++)
			if (toklist[PC+i] != vs[i])  return 0;
		return 1;
	}
	static uint16_t getaddr_v = 0;
	static char getaddr(const string& s) {
		const static string adr = "abcxyzij";
		getaddr_v = 0;
		for (int i=0; i<adr.length(); i++)
			if      (s == string()+adr[i])  return ADR_A + i;
			else if (s == strfmt("[%c]", adr[i]))  return ADRW_A + i;
		if (s == "pc")    return ADR_PC;
		if (s == "sp")    return ADR_SP;
		if (s == "[pc]")  return ADRW_PC;
		if (s == "[sp]")  return ADRW_SP;
		if (getnum(s, getaddr_v))  return ADR_NWD;
		return ADR_NIL;
	}


	static int parse_let() {
		if (string(ctok()) != "let") {
			fprintf(stderr, "let: unknown error: %s\n", ctok());
			return 1;
		}
		if (PC + 3 >= toklist.size()) {
			fprintf(stderr, "let: unexpected EOF after: %s\n", ctok());
			return 1;
		}
		char a, b;
		uint16_t aa, bb;
		// val1
		string dst = toklist[PC+1];
		if ((a = getaddr(dst)) == ADR_NIL) {
			fprintf(stderr, "error: let: unknown dest address: %s\n", dst.c_str());
			return 1;
		}
		aa = getaddr_v;
		// equals
		string eq = toklist[PC+2];
		if (eq != "=") {
			fprintf(stderr, "error: let: expected equals, got: %s\n", eq.c_str());
			return 1;
		}
		// val2
		string src = toklist[PC+3];
		if ((b = getaddr(src)) == ADR_NIL) {
			fprintf(stderr, "error: let: unknown src address: %s\n", src.c_str());
			return 1;
		}
		bb = getaddr_v;
		// add to prog
		printf("found let: %s = %s\n", dst.c_str(), src.c_str());
		prog.push_back(imerge(OP_SET, a, b));
		if (a == ADR_NWD || a == ADRW_NWD)  prog.push_back(aa);
		if (b == ADR_NWD || b == ADRW_NWD)  prog.push_back(bb);
		PC += 4;
		return 0;
	}


	static int parse_call() {
		fprintf(stderr, "TODO: call\n");
		return 1;
	}


	static int parse_func_body() {
		while (PC < toklist.size()) {
			clear_comments();
			if (tokcmp({ "end", "func" })) {
				break;
			}
			else if (string(ctok()) == "let") {
				if (parse_let())  return 1;
				continue;
			}
			else if (string(ctok()) == "call") {
				if (parse_call())  return 1;
				continue;
			}
			// unknown token
			fprintf(stderr, "error: func_body: unknown token: %s\n", ctok());
			return 1;
		}
		return 0;
	}

	
	static int parse_func() {
		string fname;
		clear_comments();
		// func title
		if (tokcmp({ "func" })) {
			fname = toklist[PC+1];
			PC += 2;
		} else {
			fprintf(stderr, "error: expected [func <name>] at: %s\n", ctok());
			return 1;
		}
		// add to list
		if (find(funclist.begin(), funclist.end(), fname) != funclist.end()) {
			printf("error: duplicate func definition: %s\n", fname.c_str());
			return 1;
		}
		printf("found func: %s\n", fname.c_str());
		funclist.push_back(fname);
		prog.insert(prog.end(), { imerge(OP_LABL, ADRW_NWD, 0), uint16_t(funclist.size()-1) });
		// parse function body
		if (parse_func_body())
			return 1;
		// function end
		if (tokcmp({ "end", "func" })) {
			PC += 2;
		} else {
			fprintf(stderr, "error: expected [end func] at: %s\n", ctok());
			return 1;
		}
		printf("end func: %s\n", fname.c_str());
		prog.push_back(imerge(OP_RET, 0, 0));
		return 0;
	}


	int fparse(const string& fname) {
		// reset
		PC = 0;
		toklist = { };
		funclist = { };
		// open
		fstream fs(fname, fstream::in);
		if (!fs.is_open()) {
			fprintf(stderr, "error loading file: %s\n", fname.c_str());
			return 1;
		}
		// load tokens
		string s, s2;
		while (fs >> s) {
			if (s[0] == '\'')  getline(fs, s2),  s += s2;
			toklist.push_back(s);
			cout << ">> " << s << endl;
		}
		// parse
		while (PC < toklist.size())
			if (parse_func())
				return 1;
		return 0;
	}

} // end basic