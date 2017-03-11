#include <iostream>
#include <string>
#include <vector>
#include <cassert>
#include <fstream>
#include "basicp.h"
#include "basiccpu.h"

using namespace std;
using namespace bc;


namespace basic {

	static const uint16_t 
		// FUNC_SIZE_POS = 5,
		FUNC_OFFSET = 32;  // function address offset (main)

	static uint16_t PC = 0;
	static vector<string> toklist;


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
	static int alen(char a) {
		return (a == ADR_NWD || a == ADRW_NWD);
	}
	static int funcindex(const string& s) {
		for (int i=0; i<funclist.size(); i++)
			if (funclist[i].first == s)
				return i;
		return -1;
	}


	static int parse_let() {
		assert(string(ctok()) == "let");  // should always be true
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
		// add to body
		printf("found let: %s = %s\n", dst.c_str(), src.c_str());
		body.push_back(imerge(OP_SET, a, b));
		if (alen(a))  body.push_back(aa);
		if (alen(b))  body.push_back(bb);
		PC += 4;
		return 0;
	}


	static int parse_call() {
		assert(string(ctok()) == "call");  // should always be true
		if (PC + 1 >= toklist.size()) {
			fprintf(stderr, "error: call: unexpected EOF after: %s\n", ctok());
			return 1;
		}
		// function name
		string fname = toklist[PC+1];
		int fi = funcindex(fname);
		// if not yet defined, add to func list
		if (fi == -1) {
			funclist.push_back({ fname, -1 });
			fi = funclist.size()-1;
		}
		// add to body
		printf("found call: %s (%d)\n", fname.c_str(), fi);
		body.insert(body.end(), { imerge(OP_JSR, ADRW_NWD, 0), uint16_t(FUNC_OFFSET + fi) });
		PC += 2;
		return 0;
	}


	static int parse_print() {
		assert(string(ctok()) == "print");  // should always be true
		if (PC + 1 >= toklist.size()) {
			fprintf(stderr, "error: print: unexpected EOF after: %s\n", ctok());
			return 1;
		}
		char a;
		int aa;
		// get print pos
		string pos = toklist[PC+1];
		if ((a = getaddr(pos)) == ADR_NIL) {
			fprintf(stderr, "error: print: unknown address: %s\n", pos.c_str());
			return 1;
		}
		aa = getaddr_v;
		// do print function
		body.insert(body.end(), {
			imerge(OP_SET, ADRW_NWD, ADR_NWD), 0x9000, DAT_VAL,  // set [0x9000] = data val
			imerge(OP_SET, ADRW_NWD, a), 0x9001,                 // set [0x9001] = number
		});
		if (alen(a))  body.push_back(aa);  // add optional inline number 
		body.insert(body.end(), { 
			imerge(OP_INT, 1, ADR_NWD), 0x9000                   // interrupt at address 9000 
		});
		// inc and move on
		PC += 2;
		return 0;
	}


	static int parse_math_in() {
		char o, b, a = getaddr(ctok());
		int bb, aa = getaddr_v;
		assert(a != ADR_NIL);  // should always be true
		// length check
		if (PC + 2 >= toklist.size()) {
			fprintf(stderr, "error: math: unexpected EOF after: %s\n", ctok());
			return 1;
		}
		// get math type
		string mtype = toklist[PC+1];
		if      (mtype == "+=")  o = OP_ADD;
		else if (mtype == "-=")  o = OP_SUB;
		else if (mtype == "*=")  o = OP_MUL;
		else if (mtype == "/=")  o = OP_DIV;
		else {
			fprintf(stderr, "error: math: expected math operation, got: %s\n", mtype.c_str());
			return 1;
		}
		// get 2nd address
		if ((b = getaddr(toklist[PC+2])) == ADR_NIL) {
			fprintf(stderr, "error: math: expected address: %s\n", toklist[PC+2].c_str());
			return 1;
		}
		bb = getaddr_v;
		// add to program body
		printf("math: [%s]  [%s]  [%s]\n", toklist[PC].c_str(), mtype.c_str(), toklist[PC+2].c_str());
		body.push_back(imerge(o, a, b));
		if (alen(a))  body.push_back(aa);
		if (alen(b))  body.push_back(bb);
		PC += 3;
		return 0;
	}


	static int parse_func_body() {
		while (PC < toklist.size()) {
			clear_comments();
			if (tokcmp({ "end", "func" })) {
				break;
			} else if (string(ctok()) == "let") {
				if (parse_let())  return 1;
				continue;
			} else if (string(ctok()) == "call") {
				if (parse_call())  return 1;
				continue;
			} else if (string(ctok()) == "print") {
				if (parse_print())  return 1;
				continue;
			} else if (getaddr(ctok()) != ADR_NIL) {
				if (parse_math_in())  return 1;
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
		// add to func list if missing
		int fi = funcindex(fname);
		if (fi == -1) {
			funclist.push_back({ fname, -1 });
			fi = funclist.size()-1;
		}
		if (funclist[fi].second == -1) {
			funclist[fi].second = body.size();
		} else {
			fprintf(stderr, "error: duplicate func definition: %s\n", fname.c_str());
			for (int i=0; i < funclist.size(); i++)
				fprintf(stderr, "  %d.  %s  %d\n", i, funclist[i].first.c_str(), funclist[i].second);
			return 1;
		}
		printf("found func: %s (%d)\n", fname.c_str(), fi);
		body.insert(body.end(), { imerge(OP_LABL, ADRW_NWD, 0), uint16_t(fi) });
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
		body.push_back(imerge(OP_RET, 0, 0));
		return 0;
	}


	static int write_head() {
		printf("writing head...\n");
		head = {
			OP_NOOP,
			imerge(OP_JSR, ADRW_NWD, 0), FUNC_OFFSET,
			imerge(OP_SET, PC, ADR_NWD), 0,
			0  // func_size (5)
		};
		while (head.size() < FUNC_OFFSET)
			head.push_back(0); // pad header
		return 0;
	}
	static int write_fhead() {
		printf("writing fhead...\n");
		fhead = { };
		// function locations
		for (int i=0; i<funclist.size(); i++) {
			if (funclist[i].second == -1) {
				fprintf(stderr, "error: write_fhead: missing function definition: %s\n", funclist[i].first.c_str());
				return 1;
			}
			printf("  %d.  %s  %d\n", i, funclist[i].first.c_str(), funclist[i].second);
			fhead.push_back( funclist[i].second );
		}
		// function names
		for (const auto& f : funclist) {
			fhead.push_back(f.first.length());
			for (auto c : f.first)
				fhead.push_back(c);
		}
		return 0;
	}
	static int write_dhead() {
		printf("writing dhead...\n");
		dhead = { 0, 0, 0, 0, 0 };  // temp
		return 0;
	}
	static int fhead_offset(uint16_t offset) {
		int len = funclist.size();
		for (int i=0; i<len; i++)
			fhead[i] += offset;
		return 0;
	}
	static int write_prog() {
		prog = { };
		prog.insert( prog.end(), head.begin(),  head.end()  );
		prog.insert( prog.end(), fhead.begin(), fhead.end() );
		prog.insert( prog.end(), dhead.begin(), dhead.end() );
		prog.insert( prog.end(), body.begin(),  body.end()  );
		return 0;
	}


	int fparse(const string& fname) {
		// reset
		PC = 0;
		toklist = { };
		// reset parser items
		funclist = { {"main", -1} };
		head = fhead = dhead = body;
		prog = { };
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
		// write headers
		if (write_head() || write_fhead() || write_dhead())  return 1;
		if (fhead_offset( head.size() + fhead.size() + dhead.size() ))  return 1;
		if (write_prog())  return 1;
		// show
		printf("=====\n");
		printf("head:\n");
		basic::showprog(head);
		printf("fhead:\n");
		basic::showprog(fhead);
		printf("dhead:\n");
		basic::showprog(dhead);
		printf("body:\n");
		basic::showprog(body);
		printf("=====\n");
		printf("prog:\n");
		basic::showprog(prog);
		return 0;
	}

} // end basic