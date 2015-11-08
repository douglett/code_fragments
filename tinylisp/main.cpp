#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <regex>
#include <cassert>

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

	enum OPCODE {
		OP_NOOP,
		OP_ADD,
		OP_SUB,
		OP_MUL,
		OP_DIV,
		OP_DEFINE
	};

	vector<string> opnames = {
		"nil", 
		"+", "-", "*", "/",
		"define"
	};

	vector<string> defnames;

	int getint(string s) {
		for (int i = 0; i < opnames.size(); i++)
			if (opnames[i].length() && opnames[i] == s)
				return i;
		for (int i = 0; i < defnames.size(); i++)
			if (defnames[i].length() && defnames[i] == s)
				return 100 + i;
		return -1;
	}

	string getstring(int id) {
		if (id >= 0 && id < opnames.size())
			if (opnames[id].length() > 0)
				return opnames[id];
		if (id >= 100 && id < 100 + defnames.size())
			if (defnames[id-100].length() > 0)
				return defnames[id-100];
		return "undefined";
	}

	int define(string s) {
		// check for existance
		for (int i = 0; i < opnames.size(); i++)
			if (opnames[i] == s)
				return i;
		for (int i = 0; i < defnames.size(); i++)
			if (defnames[i] == s)
				return 100 + i;
		// define
		defnames.push_back(s);
		return 100 + defnames.size()-1;
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
		if (pos < tokens::list.size() && id == tokens::list[pos].type)
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
				val v(val::T_INT);
				v.ival = strtoint(tokens::list[pos].val);
				// cout << v.ival << endl;
				vlist.lval.push_back(v);
				pos++;
			}
			// parse identifier / symbol
			else if (expect("identifier", pos) || expect("symbol", pos)) {
				val v(val::T_IDENT);
				v.ival = func::getint(tokens::list[pos].val);
				// cout << v.ival << "  (" << tokens::list[pos].val << ")" << endl;
				if (v.ival == -1)
					v.ival = func::define(tokens::list[pos].val);
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


	val parse_lists() {
		val vlist(val::T_LIST);
		int pos = 0;

		while (true) {
			int len = 0;
			val v = parser::parse_list(pos, len);
			if (len == -1)
				break;

			vlist.lval.push_back(v);
			pos += len;
			if (pos >= tokens::list.size())
				break;
		}

		return vlist;
	}


	void show_list(const val &vlist, int tablen = 0) {
		string tabs(tablen, '\t');
		for (const auto &v : vlist.lval) {
			switch (v.type) {
			case val::T_LIST:
				cout << tabs << "---" << endl;
				show_list(v, tablen+1);
				// cout << tabs << "---" << endl;
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

} // end parser




val eval(const val& v) {
	val rval(val::T_INT);

	if (v.type == val::T_INT)
		return v;

	else if (v.type == val::T_LIST)
		switch (v.lval[0].ival) {
		// math functions
		case func::OP_ADD:
		case func::OP_SUB:
		case func::OP_MUL:
		case func::OP_DIV:
			assert(v.lval.size() >= 2);
			rval.ival = eval(v.lval[1]).ival;  // set start value
			for (int i = 2; i < v.lval.size(); i++) {
				int res = eval(v.lval[i]).ival;
				switch (v.lval[0].ival) {
				case func::OP_ADD:
					rval.ival += res;
					break;
				case func::OP_SUB:
					rval.ival -= res;
					break;
				case func::OP_MUL:
					rval.ival *= res;
					break;
				case func::OP_DIV:
					rval.ival /= res;
					break;
				}
			}
			break;
		}

	return rval;
}




int main() {
	cout << ">> tokens:" << endl;
	parser::parse_token_file("doug.lisp");
	tokens::show();
	cout << endl;

	cout << ">> program list:" << endl;
	val vlist = parser::parse_lists();
	parser::show_list(vlist);
	cout << endl;

	cout << ">> parse:" << endl;
	for (auto v : vlist.lval)
		cout << eval(v).ival << endl;
	cout << endl;
}
