#include <iostream>
#include <regex>
#include <fstream>

using namespace std;


enum STATE {
	ST_none,
	ST_ws,
	ST_ident,
	ST_num,
	ST_string,
	ST_fslash,
	ST_comment
};

const regex 
	REG_IDENT ("[a-z_][a-z0-9_]*",		regex::icase),
	REG_NUM   ("0x[0-9a-f]|[0-9]+", 	regex::icase),
	REG_ENDL  ("[\n\r\f]", 				regex::icase);
string s = "asd\"";

string escapews(const string& s) {
	string s2;
	for (auto c : s)
		switch (c) {
		// case ' ':   s2 += " ";    break; 	// (0x20)	space (SPC)
		case '\t':	s2 += "\\t";  break; 	// (0x09)	horizontal tab (TAB)
		case '\n':  s2 += "\\n";  break;	// (0x0a)	newline (LF)
		case '\v':  s2 += "\\v";  break;	// (0x0b)	vertical tab (VT)
		case '\f':  s2 += "\\f";  break;	// (0x0c)	feed (FF)
		case '\r':  s2 += "\\r";  break;	// (0x0d)	carriage return (CR)
		default:    s2 += c;
		}
	return s2;
}

void save_tok(STATE& state, string& s) {
	string s2 = escapews(s);
	printf("  %d  [%s]\n", (int)state, s2.c_str());
	// reset state
	s = "";
	state = ST_none;
}


int main() {
	fstream fs("main.cpp");
	char c;
	string s;
	STATE state = ST_none;
	while ((c = fs.peek()) != EOF) {
		switch (state) {
		case ST_none:
			if       (isspace(c))  state = ST_ws;
			else if  (regex_match(string()+c, REG_IDENT))  state = ST_ident;
			else if  (regex_match(string()+c, REG_NUM))  state = ST_num;
			else if  (c == '"')  state = ST_string;
			else if  (c == '/')  state = ST_fslash;
			else     s = fs.get(), save_tok(state, s);  // save as unknown
			break;
		case ST_ws:
			if    (isspace(c))  s += fs.get();
			else  save_tok(state, s);
			break;
		case ST_ident:
			if    (regex_match(s+c, REG_IDENT))  s += fs.get();
			else  save_tok(state, s);
			break;
		case ST_num:
			if    (regex_match(s+c, REG_NUM))  s += fs.get();
			else  save_tok(state, s);
			break;
		case ST_string:
			s += fs.get();
			if       (s.length() == 1)  ;  // always accept first char ["]
			else if  (s.substr(s.length()-2) == "\\\"")  ;  // accept ["] if preceded by [\]
			else if  (c == '"')  save_tok(state, s);  // break on closing ["]
			break;
		case ST_fslash:
			if       (s.length() == 0)  s += fs.get();
			else if  (c == '/')  state = ST_comment;
			else if  (c == '*')  state = ST_comment;
			else     state = ST_none, save_tok(state, s);
			break;
		case ST_comment:
			s += fs.get();
			if       ( s.substr(0, 2) == "//" && regex_match(string()+c, REG_ENDL) )  save_tok(state, s);
			else if  ( s.substr(0, 2) == "/*" && s.substr(s.length()-2) == "*/" )  save_tok(state, s);
			break;
		}
	}
}