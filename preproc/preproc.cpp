#include <iostream>
#include <iomanip>
#include <map>
#include <regex>
#include <fstream>

using namespace std;


// helpers
void strtolower(string& s);
void chomp(string& s);
int  is_meta(const string& s);
string strhash(const string& s);
int  regex_replace_cb(string& line, const regex& REG, void (*cb)(string& s));
// member funcs
void label_cb(string& s);
// consts
const regex REG_IDENT      ("[a-z][a-z0-9_-]*",      regex::icase);
const regex REG_LABEL      (":[a-z_][a-z0-9_]*",     regex::icase);
const regex REG_INT        ("[-+]?[0-9]+");
// member
map<string, string> deflist;
string mpattern;



int main(int argc, char** argv) {
	// for (int i = 0; i < argc; i++)
	// 	printf("%s\n", argv[i]);
	vector<string> fnames = { "test1.dasm16", "test2.dasm16" };
	ofstream ofs("test_out.dasm16");
	// ostream& ofs = cout;
	string s;
	for (const auto& fn : fnames) {
		ifstream ifs(fn);
		// write header
		ofs << "; " << endl;
		ofs << "; ##############################" << endl;
		ofs << "; #    " << left << setw(23) << fn << " #" << endl;
		ofs << "; ##############################" << endl;
		ofs << "; " << endl;
		// 
		int lineno = 0;
		mpattern = ":__" + strhash(fn) + "_";
		while (getline(ifs, s)) {
			lineno++;
			if (is_meta(s))
				; // parse_meta(s);  // save meta-command
			else 
				regex_replace_cb(s, REG_LABEL, label_cb);  // mangle
			ofs << s << endl;  // output
		}
	}
}



//--- member funcs ---

void label_cb(string& s) {
	if (s[1] < 'A' || s[1] > 'Z')
		s = mpattern + s.substr(1);
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

// regex replace with callback, js style
int regex_replace_cb(string& line, const regex& REG, void (*cb)(string& s)) {
	int count = 0;
	smatch match;
	auto it = line.cbegin();
	string s;
	while (regex_search(it, line.cend(), match, REG)) {
		count++;
		auto& tok = match[0];
		int pos = distance(line.cbegin(), tok.first);
		s = tok.str();
		cb(s);
		line.replace(tok.first, tok.second, s);
		it = line.cbegin() + pos + s.length();
	}
	return count;
}