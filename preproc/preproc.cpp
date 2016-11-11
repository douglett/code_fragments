#include <iostream>
#include <map>
#include <regex>
#include <fstream>

using namespace std;


// header
void strtolower(string& s);
void chomp(string& s);
int  is_meta(const string& s);
string strhash(const string& s);
int replace_labels(string& line);
// consts
const regex REG_IDENT      ("[a-z][a-z0-9_-]*",      regex::icase);
const regex REG_LABEL      (":[a-z_][a-z0-9_]*",     regex::icase);
const regex REG_INT        ("[-+]?[0-9]+");
// member
map<string, string> deflist;
string mpattern;



int main() {
	ifstream ifs("test1.dasm16");
	ofstream ofs("test_out.dasm16");
	string s;
	int lineno = 0;
	mpattern = ":__" + strhash("test1.dasm16") + "_";
	while (getline(ifs, s)) {
		lineno++;
		replace_labels(s);  // mangle
		ofs << s;  // output
		if (!ifs.eof())  ofs << endl;  // manual end lines
		cout << s << ( is_meta(s) ? "  (is_meta)" : "" ) << endl;  // show
	}
}


int replace_labels(string& line) {
	int count = 0;
	smatch match;
	auto it = line.cbegin();
	while (regex_search(it, line.cend(), match, REG_LABEL)) {
		count++;
		auto& tok = match[0];
		int pos = distance(line.cbegin(), tok.first);
		string s = mpattern + tok.str().substr(1);
		// printf("-- %d  %s  %s  %d \n", pos, tok.str().c_str(), s.c_str(), (int)s.length());
		line.replace(tok.first, tok.second, s);
		it = line.cbegin() + pos + s.length();
	}
	return count;
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

// true if string begins with '#'
int is_meta(const string& s) {
	for (auto* cs = s.c_str(); *cs != '\0'; cs++) {
		if (iswspace(*cs))  continue;
		if (*cs == '#')     return 1;
		break;
	}
	return 0;
}

// short hash
string strhash(const string& s) {
	static hash<string> mhash;
	static char cs[10];
	int i = mhash("s");
	snprintf(cs, 6, "%0x", i);
	return cs;
}