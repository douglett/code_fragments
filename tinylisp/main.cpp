#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <regex>
#include <cassert>

#include "token.h"
#include "val.h"

using namespace std;



// helpers
static stringstream ss; // generic stringstream
int strtoint(string s) {
	ss.str(s), ss.clear();
	int i = 0;
	ss >> i;
	return i;
}
string inttostr(int i) {
	ss.str(""), ss.clear();
	ss << i;
	return ss.str();
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

	void tokenize_string(string tokstr, int line, int linepos) {
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
					list.push_back(Token(m, s, line, linepos+pos));
					pos += s.length();
					// cout << "\t" << s << endl;
				} else {
					c = ss.get();
					list.push_back(Token(-1, string(&c, 1), line, linepos+pos));
					pos += 1;
					// cout << "\t" << "[" << c << "]?" << endl;
				}
				s = "";
			}
		}
	}

	void tokenize_file(string fname) {
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
				tokenize_string(s, line, linepos);
				linepos = ss.tellg();
			}
		}

		f.close();
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




namespace parser {


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
				v.sval = tokens::list[pos].val;
				// cout << v.sval << endl;
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


	string show_val(const val& v) {
		ss.str(""), ss.clear();
		switch (v.type) {
		case val::T_LIST:
			if (v.lval.size() == 0)
				return "nil";
			else {
				ss << "[list x" << v.lval.size() << "]";
				return ss.str();
			}
		case val::T_INT:
			ss << v.ival;
			return ss.str();
		case val::T_IDENT:
			return string("id:") + v.sval;
		default:
			return "undefined";
		}
	}


	void show_list(const val &vlist, int tablen = 0) {
		string tabs(tablen, '\t');
		for (const auto &v : vlist.lval) {
			switch (v.type) {
			case val::T_LIST:
				cout << tabs << "---" << endl;
				show_list(v, tablen+1);
				break;
			default:
				cout << tabs << show_val(v) << endl;
			}
		}
	}

} // end parser




namespace env {

	// enum OPCODE {
	// 	OP_NOOP,
	// 	OP_ADD,
	// 	OP_SUB,
	// 	OP_MUL,
	// 	OP_DIV,
	// 	OP_DEFINE
	// };

	vector<string> opnames = {
		"nil", 
		"+", "-", "*", "/",
		"define"
	};

	map<string, val> def;

	void define(string s, val v) {
		// don't redefine opnames
		for (auto &n : opnames)
			if (n == s)
				return;
		// set def
		def[s] = v;
	}

} // end env




const val& firstitem(const val& v) {
	if (v.type == val::T_LIST && v.lval.size() > 0) {
		if (v.lval[0].type == val::T_LIST)
			return firstitem(v.lval[0]);
		else
			return v.lval[0];
	}
	return v;
}

const val& lastitem(const val& v) {
	if (v.type == val::T_LIST && v.lval.size() > 0) {
		if (v.lval.back().type == val::T_LIST)
			return lastitem(v.lval.back());
		else 
			return v.lval.back();
	}
	return v;
}


val exec(const val& call, const val& fn) {
	// parser::show_list(fn);
	return val();
}


val eval(const val& v) {
	val rval;

	if (v.type == val::T_INT)
		return v;

	else if (v.type == val::T_LIST) {
		// try and run list as function
		if (v.lval.size() > 0 && v.lval[0].type == val::T_IDENT) {
			auto name = v.lval[0].sval;
			// math functions
			if (name == "+" || name == "-" || name == "*" || name == "/") {
				assert(v.lval.size() >= 2);  // lazy error checking
				rval.type = val::T_INT;
				rval.ival = eval(v.lval[1]).ival;  // set start value
				for (int i = 2; i < v.lval.size(); i++) {
					int res = eval(v.lval[i]).ival;
					if (name == "+")
						rval.ival += res;
					else if (name == "-")
						rval.ival -= res;
					else if (name == "*")
						rval.ival *= res;
					else if (name == "/")
						rval.ival /= res;
				}
			}
			// environment operators
			else if (name == "define") {
				env::def[ v.lval[1].sval ] = v;
				return lastitem(v);
			}
			// user defined functions
			else {
				auto fn = env::def.find(name);
				if (fn != env::def.end())
					return exec(v, fn->second);
				else
					cout << "unknown exec: " << name << endl;
			}
		}
		// return last item in list
		else if (v.lval.size() > 0) {
			return eval(v.lval.back());
		}
	}

	return rval;
}




int main() {
	cout << ">> tokens:" << endl;
	tokens::tokenize_file("doug.lisp");
	tokens::show();
	cout << endl;

	cout << ">> program list:" << endl;
	val vlist = parser::parse_lists();
	parser::show_list(vlist);
	cout << endl;

	cout << ">> parse:" << endl;
	for (auto v : vlist.lval)
		cout << parser::show_val(eval(v)) << endl;
	cout << endl;
}
