#include <iostream>
#include <iomanip>
#include <map>
#include <regex>
#include <fstream>
#include <sstream>

using namespace std;


// helpers
void strtolower(string& s);
void chomp(string& s);
int  is_meta(const string& s);
string strhash(const string& s);
int  regex_replace_cb(string& line, const regex& REG, void (*cb)(string& s));
// member funcs
void label_cb(string& s);
string exec(const string& cmd);
// consts
const regex  REG_IDENT     ("[a-z][a-z0-9_-]*",      regex::icase);
const regex  REG_LABEL     (":[a-z_][a-z0-9_]*",     regex::icase);
const regex  REG_INT       ("[-+]?[0-9]+");
const string OUTPUT_DEF    ("_output.dasm16");
// member
map<string, string> deflist;
string mpattern;



int main(int argc, char** argv) {
	// figure out files to compile
	string s;
	vector<string> fnames;
	stringstream ss(exec("ls *.dasm16"));
	printf("compiling:\n");
	while (ss >> s) {
		if (s.length() >= 1 && s[0] == '_')  continue;  // ignore underscore files
		fnames.push_back(s);
		printf("    [%s]\n", s.c_str());
	}
	printf("output:\n");
	printf("    [%s]\n", OUTPUT_DEF.c_str());
	// do the work
	ofstream ofs(OUTPUT_DEF);
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
			if (is_meta(s)) {
				; // parse_meta(s);  // save meta-command
				ofs << s << endl;  // output
			} else {
				regex_replace_cb(s, REG_LABEL, label_cb);  // mangle
				ofs << s << endl;  // output
			}
		}
	}
}



//--- member funcs ---

void label_cb(string& s) {
	if (s[1] < 'A' || s[1] > 'Z')
		s = mpattern + s.substr(1);
}


string exec(const string& cmd) {
    char buffer[128];
    string result = "";
    shared_ptr<FILE> pipe(popen(cmd.c_str(), "r"), pclose);
    if (!pipe) throw runtime_error("popen() failed!");
    while (!feof(pipe.get())) {
        if (fgets(buffer, 128, pipe.get()) != NULL)
            result += buffer;
    }
    return result;
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