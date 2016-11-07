#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <regex>

using namespace std;


const regex REG_DEFINE("#define", regex::icase);
const regex REG_EXPR  ("#expr",   regex::icase);
// const regex REG_MACRO ("#macro",  regex::icase);
const regex REG_IDENT ("[a-z][a-z0-9_-]*", regex::icase);
const regex REG_INT   ("[-+]?[0-9]+");
struct deftype {
	char type;
	regex regex;
	vector<string> vs;
};
map<string, deftype> deflist;

int parse_file(char type, const string& fname);
int parse_deflist_line(int lineno, const string& ln);
int parse_input_line(int lineno, const string& ln);


//--- main ---

int main() {
	int err = 0;
	err = parse_file('d', "deflist.txt");
	err = parse_file('i', "input.txt");
	return err;
}


//--- utilities ---

void strtolower(string& s) {
	for (auto& c : s)
		c = tolower(c);
}

void chomp(string& s) {
	int  first = 0,  last = s.length();
	for (int i = 0; i < last; i++)
		if (!iswspace(s[i])) {
			first = i;
			break;
		}
	for (int i = last-1; i > first; i--)
		if (!iswspace(s[i])) {
			last = i+1;
			break;
		}
	s = s.substr(first, last-first);
}


//--- parsing ---

int parse_file(char type, const string& fname) {
	// open file
	printf("parsing: %s \n", fname.c_str());
	ifstream ifs(fname);
	if (!ifs.is_open()) {  // error checking
		fprintf(stderr, "could not open file: %s \n", fname.c_str());
		return 1;
	}
	// prepare loop
	string s;
	int lineno = 0, err = 0;
	// grab each line from file
	while (getline(ifs, s)) {
		lineno++;
		if (s.length() == 0)  continue;  // ignore blank lines
		if (s[0] == ';')      continue;  // ignore comment lines
		if (type == 'd')  parse_deflist_line(lineno, s);
		if (type == 'i')  parse_input_line(lineno, s);
		if (err)  return err;
	}
	return 0;
}


int parse_deflist_line(int lineno, const string& ln) {
	string meta, ident;
	stringstream ss(ln);
	// get meta-command
	ss >> meta;
	if (regex_match(meta, REG_DEFINE) || regex_match(meta, REG_EXPR)) {
		// check for regex name
		ss >> ident;
		if (!regex_match(ident, REG_IDENT)) {
			fprintf(stderr, "missing identifier : l%03d : [%s] \n", lineno, ident.c_str());
			return 1;
		} else if (deflist.find(ident) != deflist.end()) {
			fprintf(stderr, "identifier already defined : l%03d : [%s] \n", lineno, ident.c_str());
			return 1;
		}
		// get define regex
		if (regex_match(meta, REG_DEFINE)) {
			string mdef;
			getline(ss, mdef);
			chomp(mdef);
			deflist[ident] = { .type = 'd', .regex = regex(mdef) };
			printf("    l%03d:  [%-15s]  [%s] \n", lineno, ident.c_str(), mdef.c_str());
		}
		// get define expression
		else if (regex_match(meta, REG_EXPR)) {
			string s;
			vector<string> vs;
			while (ss >> s) {
				if (deflist.find(s) == deflist.end()) {
					fprintf(stderr, "expression contains unknown definition : l%03d : [%s] \n", lineno, ident.c_str());
					return 1;
				}
				vs.push_back(s);
			}
			deflist[ident] = { .type = 'e', .vs = vs };
			printf("    l%03d:  [%-15s]  [%d defines] \n", lineno, ident.c_str(), (int)vs.size());
		}
	} else {
		fprintf(stderr, "bad meta-command : l%03d : [%s] \n", lineno, meta.c_str());
		return 1;
	}
	return 0;  // success (destroys fstream automatically)
}


int parse_input_line(int lineno, const string& ln) {
	stringstream ss(ln);
	string tok;
	vector<pair<string, string> > toklist;
	// find all tokens
	while (ss >> tok) {
		int found = 0;
		for (const auto& d : deflist)
			if (d.second.type == 'd' && regex_match(tok, d.second.regex)) {
				toklist.push_back({ d.first, tok });
				found = 1;
				break;
			}
		if (!found) {
			fprintf(stderr, "token not found : l%03d : %s \n", lineno, tok.c_str());
			return 1;
		}
	}
	// make expressions from token list
	string en;
	for (const auto& d : deflist) {
		if (d.second.type != 'e')  continue;
		if (d.second.vs.size() != toklist.size())  continue;
		int found = 1;
		for (int i = 0; i < toklist.size(); i++)
			if (d.second.vs[i] != toklist[i].first) {
				found = 0;
				break;
			}
		if (found) {
			en = d.first;
			break;
		}
	}
	// 
	if (!en.size()) {
		fprintf(stderr, "expression not found : %03d \n", lineno);
		return 1;
	}
	printf("    l%03d:  [%-15s]  [", lineno, en.c_str());
	for (const auto& t : toklist)
		printf("%s ", t.second.c_str());
	printf("] \n");
	return 0;
}