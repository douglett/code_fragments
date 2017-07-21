#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <cassert>

using namespace std;


struct Token {
	string tok;
	int lineno, type;
};

// consts
static const string PUNC="(){}<>+-*/=%&|\\;:,.!";
// static vars
static vector<string> args;
static int lcount=0;
// global vars
vector<Token> tokens;


static int ispunc(char c) {
	for (auto cc : PUNC)
		if (c==cc)  return 1;
	return 0;
}

static void addtok(string& tok) {
	if (tok.length()==0)  return;
	tokens.push_back({ .tok=tok, .lineno=lcount, .type=0 });
	tok = "";
}

static int parsestring(const string& ln, int& pos, char qtype) {
	assert(qtype=='\'' || qtype=='"');
	string s;
	char c;
	for (int i=pos; i<ln.size(); i++) {
		c = ln[i];
		s += c;
		if (i>pos && c==qtype)  break;
	}
	if (s.length()==0 || s.back()!=qtype)
		return fprintf(stderr, "error: unterminated string on line %d\n", lcount), 1;
	pos += s.length();
	addtok(s);
	return 0;
}

static int parseline(const string& ln) {
	string s;
	char c;
	for (int i=0; i<ln.size(); i++) {
		c = ln[i];
		if      (isspace(c)) { addtok(s); }
		else if (ispunc(c))  { addtok(s);  s+=c;  addtok(s); }
		else if (c=='"' || c=='\'') { addtok(s);  if (parsestring(ln, i, c)) return 1; }
		else    { s+=c; }
	}
	addtok(s);
	return 0;
}

static int parsefile(const string& fname) {
	lcount=0;
	tokens={};
	fstream fs(fname, fstream::in);
	if (!fs.is_open())
		return fprintf(stderr, "error: could not open input file: %s\n", fname.c_str()), 1;
	string s;
	while (getline(fs, s)) {
		lcount++;
		if (parseline(s))  return 1;
	}
	return 0;
}

void showtokens() {
	printf("tokens:\n");
	for (const auto& t : tokens) {
		printf("  %d. %s\n", t.lineno, t.tok.c_str());
	}
}


int main(int argc, char** argv) {
	for (int i=0; i<argc; i++)
		args.push_back(argv[i]);

	cout << PUNC << endl;
	parsefile("main.cpp");
	showtokens();
}