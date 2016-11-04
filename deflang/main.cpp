#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <regex>

using namespace std;


const regex REG_DEFINE("#define", regex::icase);
const regex REG_MACRO ("#macro",  regex::icase);
const regex REG_IDENT ("[a-z][a-z0-9_-]*", regex::icase);
const regex REG_INT   ("[-+]?[0-9]+");
struct deftype {
	int type;
	regex regex;
	vector<string> vs;
};
map<string, deftype> deflist;


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

int parse_file(const string& fname) {
	// open file
	printf("parsing: %s \n", fname.c_str());
	ifstream ifs(fname);
	if (!ifs.is_open()) {  // error checking
		fprintf(stderr, "could not open file: %s \n", fname.c_str());
		return 1;
	}
	// prepare loop
	stringstream ss;
	string s;
	int lineno = 0;
	// grab each line from file
	while (getline(ifs, s)) {
		string meta, ident;
		lineno++;
		ss.str(s), ss.clear();
		// get meta-command
		ss >> meta;
		if (meta.length() == 0)  continue;  // ignore blank lines
		if (meta[0] == ';')      continue;  // ignore comment lines
		if (regex_match(meta, REG_DEFINE) || regex_match(meta, REG_MACRO)) {
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
				deflist[ident] = { .type = 0, .regex = regex(mdef) };
				printf("    l%03d:  [%-15s]  [%s] \n", lineno, ident.c_str(), mdef.c_str());
			}
			// get define macro
			else if (regex_match(meta, REG_MACRO)) {
				vector<string> vs;
				while (ss >> s) {
					if (deflist.find(s) == deflist.end()) {
						fprintf(stderr, "macro contains unknown definition : l%03d : [%s] \n", lineno, ident.c_str());
						return 1;
					}
					vs.push_back(s);
				}
				deflist[ident] = { .type = 1, .vs = vs };
				printf("    l%03d:  [%-15s]  [%d defines] \n", lineno, ident.c_str(), (int)vs.size());
			}
		} else {
			fprintf(stderr, "bad meta-command : l%03d : [%s] \n", lineno, meta.c_str());
			return 1;
		}
	} // end line loop
	return 0;  // success (destroys fstream automatically)
}


int main() {
	// regex r(".*asd.*");
	// cout << "regex test: " << regex_match("blhasdaf", r) << endl;
	// cout << "regex test: " << regex_match("10", REG_INT) << endl;

	int err = parse_file("deflist.txt");
	return err;
}