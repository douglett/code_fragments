#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <regex>

#include "token.h"

using namespace std;


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


void show_tokens() {
	for (auto tok : tokenlist) {
		cout << left 
			<< setw(14) << exprname(tok.type) 
			<< setw(14) << tok.val 
			<< tok.line << "-" << tok.pos 
			<< endl;
	}
}


int expect(string s, int pos) {
	int id = exprid(s);
	if (id == tokenlist[pos].type)
		return 1;
	return 0;
}


// int eval(int startpos) {
// 	int pos = startpos;
// 	expect("(", pos);
// 	expect(")", pos);
// 	return 0;
// }


class val {
public:
	enum TYPES {
		T_LIST,
		T_INT
	};
	int type = 0;
	int ival = 0;
	vector<val> lval;
};


val parse_list(int startpos, int* len) {
	int pos = startpos;
	val v;
	if (!expect("(", pos))
		return v;
	pos++;

	while (pos < tokenlist.size() && !expect(")", pos)) {
		if (expect("(", pos)) {
			int len = 0;
			parse_list(pos, &len);
			pos += len;
		} else {
			cout << tokenlist[pos].val << endl;
			pos++;
		}
	}

	if (!expect(")", pos)) {
		cout << "error: missing end bracket" << endl;
		if (len != NULL)
			*len = -1;
	} else {
		pos++;
		if (len != NULL)
			*len = pos - startpos;
	}

	return v;
}


int main() {
	cout << "hello" << endl;
	parse_token_file("doug.lisp");
	show_tokens();
	cout << endl;
	val v = parse_list(0, NULL);
	cout << endl;
}
