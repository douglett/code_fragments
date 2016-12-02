#include <iostream>
#include <regex>
#include <fstream>
#include <cassert>
#include "toktok.h"
#include "util.h"

using namespace std;
using namespace util;


// consts
static const regex 
	REG_IDENT   ("[a-z_][a-z0-9_]*",   regex::icase),
	REG_NUM     ("0x[0-9a-f]|[0-9]+",  regex::icase),
	REG_ENDL    ("[\n\r\f]",           regex::icase),
	REG_KEYWORD ("auto|void|int|char|struct|enum|const|if|else|return|"
		"for|while|switch|case|break|using|namespace",  regex::icase);



//*** helpers ***

static int peekfor(fstream& fs, string& s, const string& search) {
	// prepare
	const int len = search.length() + 1;
	char cs[len];
	s = "";
	// get
	auto pos = fs.tellg();
	fs.get(cs, len);
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



//*** parser ***

namespace toktok {

	vector<tok> toklist;


	static void save_tok(STATE& state, string& s) {
		toklist.push_back({ state, s });  // save
		s = "";  // reset state
		state = ST_none;
	}


	int parsefile(const string& fname) {
		fstream fs(fname);
		char c;
		string s;
		STATE state = ST_none;
		while ((c = fs.peek()) != EOF) {
			switch (state) {
			case ST_none:
				if       (isspace(c))  state = ST_ws;
				else if  (regex_match(string()+c, REG_IDENT))  state = ST_ident;
				else if  (regex_match(string()+c, REG_NUM))  state = ST_num;
				else if  (c == '\'')  state = ST_char;
				else if  (c == '"')   state = ST_string;
				else if  (peekmul(fs, s, { "//", "/*" }))  state = ST_comment;
				else     s = fs.get(), save_tok(state, s);  // save as unknown
				break;
			case ST_ws:
				if       (isspace(c))  s += fs.get();
				else     save_tok(state, s);
				break;
			case ST_ident:
				if       (regex_match(s+c, REG_IDENT))  s += fs.get();
				else     { if (regex_match(s, REG_KEYWORD))  state = ST_keyword;  save_tok(state, s); }  break;
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
				if       ( s.substr(0, 2) == "//" && (regex_match(string()+c, REG_ENDL) || fs.peek() == EOF) )  save_tok(state, s);
				else if  ( s.substr(0, 2) == "/*" && s.substr(s.length()-2) == "*/" )  save_tok(state, s);
				break;
			case ST_keyword:
				assert("should not happen" == 0);
			}
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

} // end toktok