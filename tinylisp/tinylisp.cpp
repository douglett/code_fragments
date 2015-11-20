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

using namespace std;
using namespace lisp;



namespace lisp {

	// consts
	const val nil;

	// static
	static stringstream ss; // generic stringstream
	static int lerror_count = 0;

	// helper members
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
	string strtolower(const string& s) {
		string s2 = s;
		for (auto &c : s2)
			c = tolower(c);
		return s2;
	}

	// error handling
	void lerror(string type, string err, const Token* tok) {
		cerr << type << " error:: " << err;
		if (tok != NULL) 
			cerr << " [" << tok->line << ":" << tok->pos << "]";
		cerr << endl;
		lerror_count++;
	}
	int haserror() {
		return lerror_count;
	}

} // end tinylisp helpers




namespace tokens {

	// consts
	const vector<vector<string> > expressions = {
		{ "(", "\\(" },
		{ ")", "\\)" },
		{ "integer", "[0-9]+" },
		{ "identifier", "[A-Za-z_][A-Za-z0-9_]*" },
		{ "symbol", ">=|<=|[\\+\\-\\/\\*=><]" },
		{ "string", "\"" },
		{ "comment", ";" }
	};
	// member vars
	vector<Token> list;

	// helpers
	static int exprid(string name) {
		for (int i = 0; i < expressions.size(); i++)
			if (expressions[i][0] == name)
				return i;
		return -1;
	}
	static string exprname(int id) {
		if (id < 0 || id >= expressions.size())
			return "undefined";
		return expressions[id][0];
	}
	static int match(string s, string e) {
		regex expr(e);
		return regex_match(s, expr);
	}
	static int match_any(string s) {
		for (int i = 0; i < expressions.size(); i++) {
			if (match(s, expressions[i][1]))
				return i;
		}
		return -1;
	}

	// parse string literal
	static string get_string(stringstream& ss, int &err) {
		string s;
		char c;
		err = 1;
		ss.get();  // clear quote
		while ((c = ss.get()) != EOF) {
			if (c == '\"') {
				err = 0; // exit clear
				break;
			} else if (c == '\\') {
				// escape characters
				c = ss.get();
				if (c == 'n')
					s += "\n";
				else if (c == 't')
					s += "\t";
				else if (c == '\"')
					s += "\"";
				else if (c == '\\')
					s += "\\";
				else
					s += c;
			} else {
				s += c;
			}
		}
		// cout << "string: " << s << endl;
		return s;
	}

	// tokenize
	int tokenize_line(string tokstr, int line) {
		stringstream ss(tokstr);
		ss >> ws;
		string s;
		char c;
		int string_id = exprid("string");
		int comment_id = exprid("comment");
		int pos = 1;

		while ((c = ss.peek()) != EOF || s.length() > 0) {
			// get token ID
			int m = tokens::match_any(s+c);
			// check for string start
			if (m == string_id) {
				int err = 0;
				s = get_string(ss, err);
				list.push_back(Token(m, s, line, pos));
				if (err) {
					lerror("tokenizer", "string error: missing end quote", &list.back());
					return 1;
				}
				// reset
				s = "";
				ss >> ws;
				pos = ss.tellg();
			}
			// check for line comment start
			else if (m == comment_id) {
				// cout << "line_comment found" << endl;
				return 0;  // nothing more needed
			}
			// while matching, continue adding chars to the token string
			else if (m != -1) {
				s += ss.get();
			} 
			// add token string to list, if a valid one was found
			else {
				if (s.length() > 0) {
					// cout << "\t" << s << endl;
					m = tokens::match_any(s);
					list.push_back(Token(m, s, line, pos));
				} else {
					lerror("tokenizer", string("unknown symbol [")+c+"]", &list.back());
					return 1;
				}
				// reset
				s = "";
				ss >> ws;
				pos = ss.tellg();
			}
		}
		return 0;  // no errors
	}

	// tokenize
	int tokenize_file(string fname) {
		fstream f(fname);
		stringstream ss;
		string s;
		int line = 0;
		int err = 0;
		// do one line at a time
		while (getline(f, s)) {
			line++;
			err = tokenize_line(s, line);
			if (err)
				break; // exit now
		}
		// cleanup
		f.close();
		return err;
	}

	// show all tokens (debugging)
	void show() {
		cout << ">> tokens:" << endl;
		for (const auto &tok : tokens::list) {
			cout << left 
				<< setw(14) << exprname(tok.type) 
				<< setw(14) << tok.val 
				<< tok.line << "-" << tok.pos 
				<< endl;
		}
		cout << endl;
	}

} // end tokens




namespace parser {

	// helpers
	static int expect(string s, int pos) {
		int id = tokens::exprid(s);
		if (pos < tokens::list.size() && id == tokens::list[pos].type)
			return 1;
		return 0;
	}

	// single list
	static val parse_list(int startpos, int& len) {
		int pos = startpos;
		val vlist;

		// find start of the list
		if (expect("(", pos)) {
			pos++;
		} else {
			lerror("parser", "missing start bracket", &tokens::list[pos-1]);
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
				v.tok = &tokens::list[pos];
				vlist.lval.push_back(v);
				pos++;
			}
			// parse identifier / symbol
			else if (expect("identifier", pos) || expect("symbol", pos)) {
				val v(val::T_IDENT);
				v.sval = strtolower(tokens::list[pos].val);
				// cout << v.sval << endl;
				v.tok = &tokens::list[pos];
				vlist.lval.push_back(v);
				pos++;
			}
			// parse string
			else if (expect("string", pos)) {
				val v(val::T_STRING);
				v.sval = tokens::list[pos].val;
				// cout << v.sval << endl;
				v.tok = &tokens::list[pos];
				vlist.lval.push_back(v);
				pos++;
			}
			// unknown
			else {
				lerror("parser", "unknown token type", &tokens::list[pos]);
				// cout << tokens::list[pos].val << " " 
				// 	<< tokens::list[pos].type << " " 
				// 	<< tokens::exprname(tokens::list[pos].type) << " "
				// 	<< endl;
				len = -1;
				return vlist;
			}
		}

		// make sure the list has an end bracket
		if (expect(")", pos)) {
			pos++;
			len = pos - startpos;
		} else {
			lerror("parser", "missing end bracket", &tokens::list[pos-1]);
			len = -1;
		}

		// return list
		return vlist;
	}

	// all lists
	val parse_lists(int &err) {
		val vlist(val::T_LIST);
		int pos = 0;
		err = 0;
		// parse each list on the top level one-by-one
		while (pos < tokens::list.size()) {
			int len = 0;
			val v = parser::parse_list(pos, len);
			if (len == -1) {  // stop on errors
				err = 1;
				break;
			}
			vlist.lval.push_back(v);
			pos += len;
		}
		return vlist; // return list of lists
	}

	// show
	string show_val(const val& v) {
		switch (v.type) {
		case val::T_LIST:
			if (v.lval.size() == 0)
				return "nil";
			return string("[list x") + inttostr(v.lval.size()) + "]";
		case val::T_INT:
			return inttostr(v.ival);
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
				show_list(v, tablen+1);
				break;
			default:
				cout << tabs << '\t' << show_val(v) << endl;
			}
		}
	}

} // end parser




namespace env {

	// const val nil;
	const vector<string> keywords = {
		"nil", 
		"+", "-", "*", "/",
		"=", ">", "<", ">=", "<=",
		"let", "defun", "print", "len", "while"
	};

	map<string, val> def;

	int define(const string& name, const val& v) {
		// don't redefine keywords
		string namel = strtolower(name);
		for (auto &s : keywords)
			if (namel == s) {
				lerror("runtime", string("tried to redefine keyword: ")+namel, v.tok);
				return 1;
			}
		// set def
		def[name] = v;
		return 0;
	}

	const val& getdef(const string& name) {
		string namel = strtolower(name);
		for (const auto &d : def)
			if (d.first == namel)
				return d.second;
		return nil;
	}

	const val& getdef(const val& name) {
		auto& v = getdef(name.sval);
		if (&v == &nil)
			lerror("runtime", string("unknown definition: ")+strtolower(name.sval), name.tok);
		return v;
	}

	void undef(const string& name) {
		string namel = strtolower(name);
		auto &v = getdef(namel);
		if (&v != &nil)
			def.erase(namel);
		// v invalid here!
	}

} // end env




namespace lisp {

	// public helpers
	int isnil(const val& v) {
		return v.type == val::T_LIST && v.lval.size() == 0;
	}
	
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

	int compare(const val& v1, const val& v2) {
		if (v1.type != v2.type)
			return 0;
		switch (v1.type) {
		case val::T_INT:
			if (v1.ival == v2.ival)
				return 1;
			break;
		case val::T_STRING:
			if (v1.sval == v2.sval)
				return 1;
			break;
		case val::T_LIST:
			if (v1.lval.size() == 0 && v2.lval.size() == 0)
				return 1;
			break;
		}
		return 0;
	}


	// execute a function
	val execute_func(const val& name, const val& args) {
		const val& func = env::getdef(name);
		if (haserror())
			return nil;
		// assert( func.lval.size() == 2 );
		if (func.lval.size() != 2) {
			lerror("runtime", "expected 2 value list", func.tok);
			return nil;
		}
		// define ids
		auto &argnames = func.lval[0].lval;
		for (int i = 0; i < argnames.size(); i++) 
			if (argnames[i].type == val::T_IDENT && i < args.lval.size()) {
				int err = env::define( argnames[i].sval, args.lval[i] );
				if (err)
					return nil;
			}
		// run
		val rval;
		for (int i = 0; i < func.lval[1].lval.size(); i++) {
			rval = eval(func.lval[1].lval[i]);
			if (haserror())
				break;
		}
		// undefine ids
		for (int i = 0; i < argnames.size(); i++)
			env::undef(argnames[i].sval);
		return rval;
	}


	// public: evaluate list
	val eval(const val& v) {
		switch (v.type) {
		case val::T_INT:
		case val::T_STRING:
			return v;

		case val::T_IDENT:
			// cout << "get " << v.sval << " " << parser::show_val(env::get(v.sval)) << endl;
			if (v.sval == "nil")
				return nil;
			return env::getdef(v);

		case val::T_LIST:
			if (isnil(v))
				return v;  // test for nil
			if (v.lval[0].type != val::T_IDENT)
				return v;  // not a function: return entire list

			// otherwise, try and run list as function
			auto name = v.lval[0].sval;

			// math functions
			if (name == "+" || name == "-" || name == "*" || name == "/") {
				assert(v.lval.size() >= 2);  // lazy error checking
				val rval(val::T_INT);
				for (int i = 1; i < v.lval.size(); i++) {
					int res = eval(v.lval[i]).ival;
					if (haserror())
						return rval;  // stop on error
					else if (i == 1)
						rval.ival = res;  // set start value
					else if (name == "+")
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
			// equality (numeric and string)
			else if (name == "=") {
				assert(v.lval.size() >= 3);
				val rval(val::T_INT);
				val comp;
				// check against each comparison value
				for (int i = 1; i < v.lval.size(); i++) {
					val vv = eval(v.lval[i]);
					if (haserror())
						return rval;  // stop on error
					else if (i == 1)
						comp = vv;  // get comparison value
					else if (!compare(comp, vv))
						return rval;  // not equal
				}
				rval.ival = 1;
				return rval;
			}
			// compare (numeric)
			else if (name == ">" || name == "<" || name == ">=" || name == "<=") {
				assert(v.lval.size() >= 3);
				val rval(val::T_INT);
				int subject = 0;
				// check against each comparison value
				for (int i = 1; i < v.lval.size(); i++) {
					int vv = eval(v.lval[i]).ival;
					if (haserror())
						return rval;  // stop on error
					else if (i == 1)
						subject = vv;  // comparison value
					else if (name == ">" && vv > subject)
						continue;
					else if (name == "<" && vv < subject)
						continue;
					else if (name == ">=" && vv >= subject)
						continue;
					else if (name == "<=" && vv <= subject)
						continue;
					else
						return rval;  // bad comparison
				}
				// all comparisons correct
				rval.ival = 1;
				return rval;
			}
			// define value
			else if (name == "let") {
				assert(v.lval.size() == 3);
				assert(v.lval[1].type == val::T_IDENT);
				const string &name = v.lval[1].sval;
				val setval = eval(v.lval[2]);
				if (haserror())
					return setval;
				env::define(name, setval);  // haserror() is set here
				return setval;
			}
			// define function
			else if (name == "defun") {
				// lazy errors
				assert(v.lval.size() == 4);
				assert(v.lval[1].type == val::T_IDENT);
				assert(v.lval[2].type == val::T_LIST);
				assert(v.lval[3].type == val::T_LIST);
				// define without
				const string &name = v.lval[1].sval;
				val deflist = sublist(v, 2, 0);
				env::define(name, deflist);  // haserror() is set here
				return lastitem(v);
			}
			// print evaluated list contents
			else if (name == "print") {
				stringstream ss;  // cache line output
				for (int i = 1; i < v.lval.size(); i++) {
					val vv = eval(v.lval[i]);
					if (haserror())
						return nil;  // stop on error
					ss << parser::show_val(vv) << " ";
				}
				cout << ss.str() << endl;
				return lastitem(v);
			}
			// list length
			else if (name == "len") {
				assert(v.lval.size() == 2);
				val len(val::T_INT);
				val arg = eval(v.lval[1]);
				if (haserror())
					return len;
				assert(arg.type == val::T_LIST);
				len.ival = arg.lval.size();
				return len;
			}
			// while loop
			else if (name == "while") {
				// cout << ">> while" << endl;
				assert(v.lval.size() == 3);
				val rval;
				auto& condition = v.lval[1];
				auto& body = v.lval[2].lval;
				while (eval(condition).ival != 0) {
					if (haserror())
						return nil;
					for (int i = 0; i < body.size(); i++) {
						rval = eval(body[i]);
						if (haserror())
							return nil;
					}
				}
				return rval;  // return last result
			}
			// user defined functions
			else {
				// cout << ">> calling: " << name << endl;
				val args(val::T_LIST);
				for (int i = 0; i < v.lval.size(); i++) {
					val vv = eval(v.lval[i]);
					if (haserror())
						return nil;  // error check everything, including first item
					if (i >= 1)
						args.lval.push_back(vv);
				}
				return execute_func(v.lval[0], args);
			}
		} // end switch

		cout << "should never get here" << endl;
		return nil;
	}

} // end lisp
