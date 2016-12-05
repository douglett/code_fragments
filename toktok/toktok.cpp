#include <iostream>
#include <vector>
#include <cassert>
#include <fstream>
#include <regex>
#include "toktok.h"

using namespace std;
using namespace util;


// consts
static const regex 
	REG_IDENT   ("[A-Za-z_][A-Za-z0-9_]*",  regex::icase),  // icase not working on rpi...
	REG_NUM     ("0x[0-9A-Fa-f]|[0-9]+",    regex::icase),
	REG_ENDL    ("[\n\r\f]",                regex::icase);
// language def
const tok::lang_t 
	tok::LANG_DEFAULT = {
		.lnstart = "//", .blstart = "/*", .blend = "*/"
	},
	tok::LANG_CPP = {
		.lnstart = "//", .blstart = "/*", .blend = "*/",
		.keyw = {
			"auto", "void", "int", "char",
			"struct", "enum", "const", "static", "if", "else", "return",
			"for", "while", "switch", "case", "break", "using", "namespace"
		}
	},
	tok::LANG_QBASIC = {
		.lnstart = "'", .blstart = "/*", .blend = "*/",
		.keyw = {
			"defint", "declare", "sub", "function", "end", "dim", "type", "as", "const",
			"if", "then", "else", "not", 
			"gosub", "return", "while", "wend", "do", "loop", "for", "next", "select", "case",
			"integer", "string", "float",
			"def", "seg", "peek", "poke"
		}
	};



//*** helpers ***

static int peekfor(fstream& fs, string& s, const string& search) {
	if (search.length() == 0)  return 0;
	// prepare
	const int len = search.length() + 1;
	char cs[len];
	s = "";
	// get
	auto pos = fs.tellg();
	fs.get(cs, len);
	// printf("[[%s]]\n", cs);
	// result
	if (search == cs) {
		s = cs;
		return 1;
	} else {
		fs.seekg(pos);
		return 0;
	}
}

static int peekmul(fstream& fs, string& s, const vector<string>& search) {
	for (const auto ser : search)
		if (peekfor(fs, s, ser))
			return 1;
	return 0;
}

static int findkeyw(const vector<string> keyw, const string& s) {
	//cout << "keyw: " << s << endl;
	for (const auto& k : keyw)
		if (strtolower(k) == strtolower(s))
			return 1;
	return 0;
}



//*** parser ***

namespace tok {

	vector<tok>  toklist;
	lang_t       lang = LANG_DEFAULT;


	static void save_tok(STATE& state, string& s) {
		toklist.push_back({ state, s });  // save
		s = "";  // reset state
		state = ST_none;
	}


	int parsefile(const string& fname) {
		fstream fs(fname);
		string s;
		STATE state = ST_none;
		while (true) {
			char c = fs.peek();
			switch (state) {
			case ST_none:
				if       (isspace(c))  state = ST_ws;
				else if  (peekmul(fs, s, { lang.lnstart, lang.blstart }))  state = ST_comment;
				else if  (c == '\'')  state = ST_char;
				else if  (c == '"')   state = ST_string;
				else if  (regex_match(string()+c, REG_NUM))  state = ST_num;
				else if  (regex_match(string()+c, REG_IDENT))  state = ST_ident;
				else if  (c != char(EOF))  s = fs.get(), save_tok(state, s);  // save as unknown
				break;
			case ST_ws:
				if       (isspace(c))  s += fs.get();
				else     save_tok(state, s);
				break;
			case ST_ident:
				if       (regex_match(s+c, REG_IDENT))  s += fs.get();
				else     { if (findkeyw(lang.keyw, s))  state = ST_keyword;  save_tok(state, s); }  break;
			case ST_num:
				if       (regex_match(s+c, REG_NUM))  s += fs.get();
				else     save_tok(state, s);
				break;
			case ST_char:
				s += fs.get();
				if       (s.length() == 1)  ;  // accept first
				else if  (s.substr(s.length()-2) == "\\'")  ;  // accept escaped s-quote
				else if  (c == '\'')  save_tok(state, s);
				break;
			case ST_string:
				s += fs.get();
				if       (s.length() == 1)  ;  // always accept first char ["]
				else if  (s.substr(s.length()-2) == "\\\"")  ;  // accept ["] if preceded by [\]
				else if  (c == '"')  save_tok(state, s);  // break on closing ["]
				break;
			case ST_comment:
				s += fs.get();
				if       ( s.substr(0, lang.lnstart.length()) == lang.lnstart && regex_match(string()+c, REG_ENDL) )  save_tok(state, s);
				else if  ( s.substr(0, lang.blstart.length()) == lang.blstart && s.substr(s.length()-2) == lang.blend )  save_tok(state, s);
				break;
			case ST_keyword:
				assert("should not happen" == 0);
			}
			// exit on EOF
			if (c == char(EOF))  break;
		}
		// check if we missed some tokens
		if (s.length()) {
			cout << "\n*** ERROR  (state: " << state << ") ***\n\n" << s << endl;
			return 1;
		}
		// all ok
		return 0;
	}


	void show(char fmt) {
		switch (fmt) {
		case 'c':
			for (const auto& t : toklist) {
				switch (t.state) {
				case ST_ident:
				case ST_ws:
				case ST_none:     cout << ccode(DEFAULT);  break;
				case ST_keyword:  cout << ccode(RED);  break;
				case ST_num:
				case ST_char:     cout << ccode(PURPLE);  break;
				case ST_string:   cout << ccode(YELLOW);  break;
				case ST_comment:  cout << ccode(GRAY);  break;
				}
				cout << t.str.c_str();
			}
			cout << ccode(DEFAULT) << endl;
			break;
		case 'l':
		default:
			for (const auto& t : toklist)
				printf("  %d  [%s]\n", (int)t.state, escapews(t.str).c_str());
			break;
		}
	}

} // end tok
