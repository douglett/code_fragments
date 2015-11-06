#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <regex>

#include "token.h"
#include "val.h"

using namespace std;



// helpers
int strtoint(string s) {
	static stringstream ss;
	ss.str(s), ss.clear();
	int i = 0;
	ss >> i;
	return i;
}



vector<Token> tokenlist;

vector<vector<string> > token_expressions = {
	{ "(", "\\(" },
	{ ")", "\\)" },
	{ "integer", "[0-9]+" },
	{ "identifier", "[A-Za-z_][A-Za-z0-9_]*" },
	{ "symbol", "[\\+\\-\\/\\*]" }
};

int exprid(string name) {
	for (int i = 0; i < token_expressions.size(); i++)
		if (token_expressions[i][0] == name)
			return i;
	return -1;
}
string exprname(int id) {
	if (id < 0 || id >= token_expressions.size())
		return "undefined";
	return token_expressions[id][0];
}

int match(string s, string e) {
	regex expr(e);
	return regex_match(s, expr);
}

int match_any(string s) {
	for (int i = 0; i < token_expressions.size(); i++) {
		if (match(s, token_expressions[i][1]))
			return i;
	}
	return -1;
}



const int FUNC_NAMES_MAX = 200;
string func_names[FUNC_NAMES_MAX] = {
	"+", "-", "*", "/"
};

int func_getint(string s) {
	for (int i = 0; i < FUNC_NAMES_MAX; i++)
		if (func_names[i].length() && func_names[i] == s)
			return i;
	return -1;
}

string func_getstring(int id) {
	if (id >= 0 && id < FUNC_NAMES_MAX && func_names[id].length() > 0)
		return func_names[id];
	return "undefined";
}



void parse_token_string(string tokstr, int line, int linepos) {
	stringstream ss(tokstr);
	string s;
	char c;
	int pos = 1;

	while ((c = ss.peek()) != EOF || s.length() > 0) {
		// cout << c << endl;
		int m = match_any(s+c);
		if (m != -1) {
			s += ss.get();
		} else {
			if (s.length() > 0) {
				m = match_any(s);
				tokenlist.push_back(Token(m, s, line, linepos+pos));
				pos += s.length();
				// cout << "\t" << s << endl;
			} else {
				c = ss.get();
				tokenlist.push_back(Token(-1, string(&c, 1), line, linepos+pos));
				pos += 1;
				// cout << "\t" << "[" << c << "]?" << endl;
			}
			s = "";
		}
	}
}


void parse_token_file(string fname) {
	fstream f(fname);
	stringstream ss;
	string s;
	int line = 0;

	while (getline(f, s)) {
		line++;
		ss.str(s), ss.clear();
		int linepos = 1;
		while (ss >> s) {
			// printf("%d 	%d\n", line, linepos);
			parse_token_string(s, line, linepos);
			linepos = ss.tellg();
		}
	}

	f.close();
}




int expect(string s, int pos) {
	int id = exprid(s);
	if (id == tokenlist[pos].type)
		return 1;
	return 0;
}


val parse_list(int startpos, int& len) {
	int pos = startpos;
	val vlist;

	// find start of the list
	if (expect("(", pos)) {
		pos++;
	} else {
		cout << "error: missing start bracket" << endl;
		len = -1;
		return vlist;
	}

	// parse each item in the list
	while (pos < tokenlist.size() && !expect(")", pos)) {
		// try and parse a sub-list
		if (expect("(", pos)) {
			int sublen = 0;
			vlist.lval.push_back( parse_list(pos, sublen) );
			if (sublen == -1) {
				len = -1;
				return vlist;
			}
			pos += sublen;
		} 
		// parse an integer
		else if (expect("integer", pos)) {
			val v;
				v.type = val::T_INT;
				v.ival = strtoint(tokenlist[pos].val);
				// cout << v.ival << endl;
			vlist.lval.push_back(v);
			pos++;
		}
		// parse identifier / symbol
		else if (expect("identifier", pos) || expect("symbol", pos)) {
			val v;
				v.type = val::T_IDENT;
				v.ival = func_getint(tokenlist[pos].val);
				// cout << v.ival << "  (" << tokenlist[pos].val << ")" << endl;
			vlist.lval.push_back(v);
			pos++;
		}
	}

	// make sure the list has an end bracket
	if (expect(")", pos)) {
		pos++;
		len = pos - startpos;
	} else {
		cout << "error: missing end bracket" << endl;
		len = -1;
	}

	// return list
	return vlist;
}




void show_tokens() {
	for (const auto &tok : tokenlist) {
		cout << left 
			<< setw(14) << exprname(tok.type) 
			<< setw(14) << tok.val 
			<< tok.line << "-" << tok.pos 
			<< endl;
	}
}

void show_list(const val &vlist, int tablen = 0) {
	string tabs(tablen, '\t');
	for (const auto &v : vlist.lval) {
		switch (v.type) {
		case val::T_LIST:
			show_list(v, tablen+1);
			break;
		case val::T_INT:
			cout << tabs << v.ival << endl;
			break;
		case val::T_IDENT:
			cout << tabs << func_getstring(v.ival) << endl;
			break;
		}
	}
}




int main() {
	cout << ">> tokens:" << endl;
	parse_token_file("doug.lisp");
	show_tokens();
	cout << endl;

	cout << ">> program list:" << endl;
	int err = 0;
	val v = parse_list(0, err);
	show_list(v);
	cout << endl;
}
