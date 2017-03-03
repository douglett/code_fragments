#include <iostream>
#include <string>
#include <fstream>

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
	static char addr(const string& t) {
		// static const regex REG_NUMBER("[0-9]+");
		// val_t = 0;
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

	//--- load to CPU struct ---
	int load_parse(const string& fname, CPU& cpu) {
		reset(cpu);
		vector<string> tok;
		if (load_tok(fname, tok))  return 1;
		// memcpy(cpu.ram, &prog[0], sizeof(uint16_t) * prog.size());
		return 0;
	}

} // end parse
} // end bc