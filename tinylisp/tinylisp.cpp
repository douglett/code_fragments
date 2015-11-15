#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <regex>
#include <cassert>

#include "tinylisp.h"
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

	const vector<vector<string> > expressions = {
		{ "(", "\\(" },
		{ ")", "\\)" },
		{ "integer", "[0-9]+" },
		{ "identifier", "[A-Za-z_][A-Za-z0-9_]*" },
		{ "symbol", "[\\+\\-\\/\\*]" },
		{ "string", "\"" }
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

	string get_string(stringstream& ss, int &err) {
		string s;
		char c;
		err = 0;
		
		ss.get();  // clear quote
		while (true) {
			c = ss.get();
			if (c == EOF) {
				cerr << "string error: missing end quote" << endl;
				err = 1;
				break;
			}
			if (c == '\"') 
				break;
			s += c;
		}
		// cout << "string: " << s << endl;
		return s;
	}

	int tokenize_string(string tokstr, int line, int linepos) {
		stringstream ss(tokstr);
		string s;
		char c;
		int stringid = exprid("string");
		int pos = 1;

		while ((c = ss.peek()) != EOF || s.length() > 0) {
			// cout << c << endl;
			int m = tokens::match_any(s+c);
			if (m == stringid) {
				int err = 0;
				s = get_string(ss, err);
				if (err)
					return 1;
				list.push_back(Token(m, s, line, linepos+pos));
				pos += s.length();
				s = "";
			} else if (m != -1) {
				s += ss.get();
			} else {
				if (s.length() > 0) {
					m = tokens::match_any(s);
					list.push_back(Token(m, s, line, linepos+pos));
					pos += s.length();
					// cout << "\t" << s << endl;
				} else {
					// c = ss.get();
					// list.push_back(Token(-1, string(&c, 1), line, linepos+pos));
					// pos += 1;
					cerr << "tokenizer error: unknown symbol [" << c << "]" << endl;
					return 1;
				}
				s = "";
			}
		}
		return 0;
	}

	int tokenize_file(string fname) {
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
				int err = tokenize_string(s, line, linepos);
				if (err)
					return 1;
				linepos = ss.tellg();
			}
		}

		f.close();
		return 0;
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
			cerr << "parser error: missing start bracket" << endl;
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
			// parse string
			else if (expect("string", pos)) {
				val v(val::T_STRING);
				v.sval = tokens::list[pos].val;
				vlist.lval.push_back(v);
				pos++;
			}
			// unknown
			else {
				cerr << "parser error: unknown token type: " << tokens::list[pos].val << endl;
				len = -1;
				return vlist;
			}
		}

		// make sure the list has an end bracket
		if (expect(")", pos)) {
			pos++;
			len = pos - startpos;
		} else {
			cerr << "parser error: missing end bracket" << endl;
			len = -1;
		}

		// return list
		return vlist;
	}


	val parse_lists(int &err) {
		val vlist(val::T_LIST);
		int pos = 0;
		err = 0;

		while (true) {
			int len = 0;
			val v = parser::parse_list(pos, len);
			if (len == -1) {
				err = 1;
				break;
			}

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
		case val::T_STRING:
			return v.sval;
		default:
			return "undefined";
		}
	}


	void show_list(const val &vlist, int tablen) {
		string tabs(tablen, '\t');
		cout << tabs << "---" << endl;
		for (const auto &v : vlist.lval) {
			switch (v.type) {
			case val::T_LIST:
				// cout << tabs << "---" << endl;
				show_list(v, tablen+1);
				break;
			default:
				cout << tabs << '\t' << show_val(v) << endl;
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

	const val nil = val();
	const vector<string> opnames = {
		"nil", 
		"+", "-", "*", "/",
		"define", "print"
	};

	map<string, val> def;

	void define(string s, const val& v) {
		// don't redefine opnames
		for (auto &n : opnames)
			if (n == s)
				return;
		// set def
		def[s] = v;
	}

	const val& get(string s) {
		for (const auto &d : def)
			if (d.first == s)
				return d.second;
		return nil;
	}

	void undef(string s) {
		auto &v = get(s);
		if (&v != &nil)
			def.erase(s);
		// v invalid here!
	}

} // end env




val eval(const val& v);

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

const val& atomize(const val& v) {
	if (v.type == val::T_LIST && v.lval.size() == 1)
		return atomize(v.lval[0]);
	return v;
}

val sublist(const val& v, int start, int len) {
	int last = start + len;
	if (len < 1)
		last = v.lval.size() + len;
	val rval;
	for (int i = start; i < last; i++)
		rval.lval.push_back(v.lval[i]);
	return atomize(rval);
}


val exec(const val& call, const val& fn) {
	assert(fn.lval.size() >= 2);
	// define ids
	auto &args = fn.lval[0].lval;
	for (int i = 0; i < args.size(); i++) 
		if (args[i].type == val::T_IDENT && i+1 < call.lval.size())
			env::define(args[i].sval, call.lval[i+1]);
	// run
	val rval;
	for (int i = 1; i < fn.lval.size(); i++)
		rval = eval(fn.lval[i]);
	// undefine ids
	for (int i = 0; i < args.size(); i++) 
		env::undef(args[i].sval);
	return rval;
}


val eval(const val& v) {
	switch (v.type) {
	case val::T_INT:
	case val::T_STRING:
		return v;

	case val::T_IDENT:
		// cout << "get " << v.sval << " " << parser::show_val(env::get(v.sval)) << endl;
		return env::get(v.sval);

	case val::T_LIST:
		// test for nil
		if (v.lval.size() == 0)
			return v;
		// not a function: return entire list
		if (v.lval[0].type != val::T_IDENT)
			return v;

		// otherwise, try and run list as function
		auto name = v.lval[0].sval;

		// math functions
		if (name == "+" || name == "-" || name == "*" || name == "/") {
			assert(v.lval.size() >= 2);  // lazy error checking
			val rval(val::T_INT);
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
			return rval;
		}
		// environment: define item
		else if (name == "define") {
			assert(v.lval.size() >= 3);
			const string &name = v.lval[1].sval;
			val deflist = sublist(v, 2, 0);
			env::define(name, deflist);
			return lastitem(v);
		}
		// environment: print list
		else if (name == "print") {
			assert(v.lval.size() >= 2);
			for (int i = 1; i < v.lval.size(); i++)
				cout << parser::show_val( eval(v.lval[i]) ) << " ";
			cout << endl;
			return lastitem(v);
		}
		// user defined functions
		else {
			// cout << "calling: " << name << endl;
			auto &fn = env::get(name);
			if (fn.type == val::T_LIST && fn.lval.size() > 0)
				return exec(v, fn);
			// not defined
			cerr << "unknown exec: " << name << endl;
			return val();
		}
	} // end switch

	// should never get here
	return val();
}
