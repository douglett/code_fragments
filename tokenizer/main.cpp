#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <cassert>

using namespace std;


enum TOK_TYPE {
	TOK_NONE=0,
	TOK_STRING,
	TOK_CHAR,
	TOK_PUNC,
	TOK_IDENT,
	TOK_NUM,
	TOK_COMMENT
};
struct Token {
	string tok;
	int lineno, type;
};

// consts
static const string PUNC="(){}<>[]+-*/=%&|\\;:,.!";
static const string LINE_COMMENT="//";
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
static int isident(const string& s) {
	char c;
	for (int i=0; i<s.length(); i++) {
		c = s[i];
		if ((c>='a' && c<='z') || (c>='A' && c<='Z') || c=='_')  continue;
		if (i>0 && c>='0' && c<='9')  continue;
		return 0;
	}
	return 1;
}
static int iscomment(const string& s, int pos) {
	if (s.substr(pos, LINE_COMMENT.length()) == LINE_COMMENT)
		return 1;
	return 0;
}

static string type_name(int t) {
	switch (TOK_TYPE(t)) {
	case TOK_NONE:     return "NONE";
	case TOK_STRING:   return "STRING";
	case TOK_CHAR:     return "CHAR";
	case TOK_PUNC:     return "PUNC";
	case TOK_IDENT:    return "IDENT";
	case TOK_NUM:      return "NUM";
	case TOK_COMMENT:  return "COMMEN";
	}
	return "???";
}

static void addtok(string& tok) {
	if (tok.length()==0)  return;
	int type = TOK_NONE;
	if      (iscomment(tok,0))  type=TOK_COMMENT;
	else if (tok[0]=='"')       type=TOK_STRING;
	else if (tok[0]=='\'')      type=TOK_CHAR;
	else if (ispunc(tok[0]))    type=TOK_PUNC;
	else if (isident(tok))      type=TOK_IDENT;
	tokens.push_back({ .tok=tok, .lineno=lcount, .type=type });
	tok = "";
}

static int parsestring(const string& ln, int pos, char qtype) {
	assert(qtype=='\'' || qtype=='"');
	string s;
	char c;
	for (int i=pos; i<ln.size(); i++) {
		c = ln[i];
		s += c;
		if (i>pos && c==qtype)  break;
	}
	if (s.length()==0 || s.back()!=qtype)
		return fprintf(stderr, "error: unterminated string on line %d\n", lcount), 0;
	// pos += s.length();
	int len = s.length();
	addtok(s);  // note: resets s
	return len;
}

static int parseline(const string& ln) {
	string s;
	char c;
	for (int i=0; i<ln.size(); i++) {
		c = ln[i];
		if      (isspace(c)) { addtok(s); }
		else if (iscomment(ln,i))  { addtok(s);  s=ln.substr(i);  break; }
		else if (c=='"' || c=='\'') { addtok(s);  int len=parsestring(ln, i, c);  if (len==0) return 1;  i+=len; }
		else if (ispunc(c))  { addtok(s);  s+=c;  addtok(s); }
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
	int lcount=0;
	for (const auto& t : tokens) {
		if (t.lineno > lcount)  printf("  [%d]\n", (lcount=t.lineno));
		// printf("    %s  (%d)\n", t.tok.c_str(), t.type);
		printf("    (%6s)  %s\n", type_name(t.type).c_str(), t.tok.c_str());
	}
}


int main(int argc, char** argv) {
	for (int i=0; i<argc; i++)
		args.push_back(argv[i]);

	cout << PUNC << endl;
	parsefile("main.cpp");
	showtokens();
}