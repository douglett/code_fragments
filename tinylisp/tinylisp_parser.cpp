#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <regex>

#include "tinylisp.h"

using namespace std;
using namespace lisp;



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
		// open check
		fstream f(fname, fstream::in);
		if (!f.is_open()) {
			lerror("tokenizer", string("file not found: ") + fname, NULL);
			return 1;
		}
		// setup
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
