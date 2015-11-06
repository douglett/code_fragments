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



namespace tokens {

	vector<Token> list;

	vector<vector<string> > expressions = {
		{ "(", "\\(" },
		{ ")", "\\)" },
		{ "integer", "[0-9]+" },
		{ "identifier", "[A-Za-z_][A-Za-z0-9_]*" },
		{ "symbol", "[\\+\\-\\/\\*]" }
	};

	int exprid(string name) {
		for (int i = 0; i < expressions.size(); i++)
			if (expressions[i][0] == name)
				return i;
		return -1;
	}
	string exprname(int id) {
		if (id < 0 || id >= expressions.size())
			return "undefined";
		return expressions[id][0];
	}

	int match(string s, string e) {
		regex expr(e);
		return regex_match(s, expr);
	}

	int match_any(string s) {
		for (int i = 0; i < expressions.size(); i++) {
			if (match(s, expressions[i][1]))
				return i;
		}
		return -1;
	}

	void show() {
		for (const auto &tok : tokens::list) {
			cout << left 
				<< setw(14) << exprname(tok.type) 
				<< setw(14) << tok.val 
				<< tok.line << "-" << tok.pos 
				<< endl;
		}
	}

} // end tokens




namespace func {

	const int FUNC_NAMES_MAX = 200;
	string names[FUNC_NAMES_MAX] = {
		"+", "-", "*", "/"
	};

	int getint(string s) {
		for (int i = 0; i < FUNC_NAMES_MAX; i++)
			if (names[i].length() && names[i] == s)
				return i;
		return -1;
	}

	string getstring(int id) {
		if (id >= 0 && id < FUNC_NAMES_MAX && names[id].length() > 0)
			return names[id];
		return "undefined";
	}

} // end func




namespace parser {

	void parse_token_string(string tokstr, int line, int linepos) {
		stringstream ss(tokstr);
		string s;
		char c;
		int pos = 1;

		while ((c = ss.peek()) != EOF || s.length() > 0) {
			// cout << c << endl;
			int m = tokens::match_any(s+c);
			if (m != -1) {
				s += ss.get();
			} else {
				if (s.length() > 0) {
					m = tokens::match_any(s);
					tokens::list.push_back(Token(m, s, line, linepos+pos));
					pos += s.length();
					// cout << "\t" << s << endl;
				} else {
					c = ss.get();
					tokens::list.push_back(Token(-1, string(&c, 1), line, linepos+pos));
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
		int id = tokens::exprid(s);
		if (id == tokens::list[pos].type)
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
		while (pos < tokens::list.size() && !expect(")", pos)) {
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
					v.ival = strtoint(tokens::list[pos].val);
					// cout << v.ival << endl;
				vlist.lval.push_back(v);
				pos++;
			}
			// parse identifier / symbol
			else if (expect("identifier", pos) || expect("symbol", pos)) {
				val v;
					v.type = val::T_IDENT;
					v.ival = func::getint(tokens::list[pos].val);
					// cout << v.ival << "  (" << tokens::list[pos].val << ")" << endl;
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

} // end parser




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
			cout << tabs << func::getstring(v.ival) << endl;
			break;
		}
	}
}



int main() {
	cout << ">> tokens:" << endl;
	parser::parse_token_file("doug.lisp");
	tokens::show();
	cout << endl;

	cout << ">> program list:" << endl;
	int err = 0;
	val v = parser::parse_list(0, err);
	show_list(v);
	cout << endl;
}
