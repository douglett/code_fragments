#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <regex>

#include "symbol.h"

using namespace std;


vector<Symbol> symbollist;

vector<vector<string> > symbol_expressions = {
	{ "(", "\\(" },
	{ ")", "\\)" },
	{ "integer", "[0-9]+" },
	{ "identifier", "[A-Za-z_][A-Za-z0-9_]*" },
	{ "symbol", "[\\+\\-\\/\\*]" }
};


int exprid(string name) {
	for (int i = 0; i < symbol_expressions.size(); i++)
		if (symbol_expressions[i][0] == name)
			return i;
	return -1;
}
string exprname(int id) {
	if (id < 0 || id >= symbol_expressions.size())
		return "undefined";
	return symbol_expressions[id][0];
}


int match(string s, string e) {
	regex expr(e);
	return regex_match(s, expr);
}


int match_any(string s) {
	for (int i = 0; i < symbol_expressions.size(); i++) {
		if (match(s, symbol_expressions[i][1]))
			return i;
	}
	return -1;
}


void parse_symbols_string(string symbolstr) {
	stringstream ss(symbolstr);
	string s;
	char c;

	while ((c = ss.peek()) != EOF || s.length() > 0) {
		// cout << c << endl;
		int m = match_any(s+c);
		if (m != -1) {
			s += ss.get();
		} else {
			if (s.length() > 0) {
				// cout << "\t" << s << endl;
				m = match_any(s);
				symbollist.push_back(Symbol(m, s));
			} else {
				c = ss.get();
				// cout << "\t" << "[" << c << "]?" << endl;
				symbollist.push_back(Symbol(-1, string(&c, 1)));
			}
			s = "";
		}
	}
}


void parse_symbols(string fname) {
	fstream f(fname);
	string s;

	while (f >> s) {
		parse_symbols_string(s);
	}

	f.close();
}


void show_symbols() {
	for (auto sym : symbollist) {
		cout << setw(10) << exprname(sym.type) << " : " << sym.val << endl;
	}
}


int expect(string s, int pos) {
	int id = exprid(s);
	if (id == symbollist[pos].type)
		return 1;
	return 0;
}


int eval(int startpos) {
	int pos = startpos;
	expect("(", pos);
	expect(")", pos);
}


int main() {
	cout << "hello" << endl;
	parse_symbols("doug.lisp");
	show_symbols();
	eval(0);
}
