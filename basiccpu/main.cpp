#include <iostream>
#include <vector>
#include <fstream>
#include <regex>
#include <cassert>
#include "basiccpu.h"

using namespace std;
using namespace bc;

void test1();
void testparse();
vector<uint16_t> parse(const string& fname);
void showprog(const vector<uint16_t>& prog);


int main() {
	printf("start\n");
	// test1();
	testparse();
}


void test1() {
	CPU cpu;
	reset(cpu);
	vector<uint16_t> prog;
	prog.insert(prog.end(), { 
		imerge(OP_SET, ADR_A, ADR_NWD), 10,
		imerge(OP_ADD, ADR_A, ADR_NWD), 2,
		imerge(OP_JSR, ADR_NWD, 0), 20
	});
	memcpy(cpu.ram, &prog[0], prog.size() * sizeof(uint16_t));
	while (step(cpu) == 0)  ;
	printf("cycles: %d\n", cpu.CYC);
	printf("A: %d \n", cpu.A);
}

const regex REG_NUMBER("[0-9]+");
uint16_t val_t;

char addr(const string& t) {
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

void testparse() {
	auto prog = parse("example.bas");
	showprog(prog);

	CPU cpu;
	reset(cpu);
	// memcpy(cpu.ram, &prog[0], prog.size() * sizeof(uint16_t));
	// while (step(cpu) == 0)  ;
}

vector<uint16_t> parse(const string& fname) {
	static const int DISPLAY = 0;
	// load from file
	vector<string> tok;
	string s;
	fstream fs(fname, fstream::in);
	while (!fs.eof()) {
		fs >> ws;
		if      (fs.peek() == '\'')  { getline(fs, s);  tok.push_back(s); }
		else if (fs >> s)  { tok.push_back(s); }
	}
	fs.close();
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
	vector<uint16_t> prog;
	// parse
	for (int i = 0; i < tok.size(); i++) {
		if (tok[i] == "if") {
			// arg1
			a = addr(tok[++i]),  aa = val_t;
			// comparison
			t = tok[++i];
			if      (t == "=")   o = OP_IFE;
			else if (t == "==")  o = OP_IFE;
			else if (t == "!=")  o = OP_IFN;
			// arg2
			b = addr(tok[++i]),  bb = val_t;
			// decoration
			assert(tok[++i] == "then");
		} 
		else if (tok[i] == "end") {
			assert(tok[++i] == "if");
		}
		else if (tok[i] == "let") {
			// arg1
			a = addr(tok[++i]),  aa = val_t;
			// equals
			assert(tok[++i] == "=");
			o = OP_SET;
			// arg2
			b = addr(tok[++i]),  bb = val_t;
		} 
		else if (tok[i] == "add") {
			o = OP_ADD;
			// arg1
			a = addr(tok[++i]),  aa = val_t;
			// arg2
			b = addr(tok[++i]),  bb = val_t;
		}
		else if (tok[i] == "print") {
			o = OP_PRNT;
			a = addr(tok[++i]),  aa = val_t;
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
	// return
	return prog;
}


void showprog(const vector<uint16_t>& prog) {
	char o, a, b;
	for (int i = 0; i < prog.size(); i++) {
		isplit(prog[i], &o, &a, &b);
		string out = strfmt("%x %x %x", o, a, b);
		if (a == ADR_NWD || a == ADRW_NWD || b == ADR_NWD || b == ADRW_NWD) {
			out += "  (";
			if (a == ADR_NWD || a == ADRW_NWD)  out += strfmt("%d  ", prog[++i]);
			if (b == ADR_NWD || b == ADRW_NWD)  out += strfmt("%d",   prog[++i]);
			out += ")";
		}
		cout << out << endl;
	}
}