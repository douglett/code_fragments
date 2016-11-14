#include <iostream>
#include <iomanip>
#include <map>
#include <regex>
#include <fstream>
#include <sstream>

using namespace std;


// consts
// const regex  REG_INT       ("[-+]?[0-9]+");
// const regex  REG_IDENT     ("[a-z][a-z0-9_-]*",    regex::icase);
static const regex  REG_LABEL     (":[a-z_][a-z0-9_]*",               regex::icase);
static const regex  REG_INCLUDE   ("^\\s*#include\\s+\"(.+)\"\\s*$",  regex::icase);
static const string OUTPUT_DEF    ("_output.dasm16");
// members
// map<string, string> deflist;
static string mpattern;



//--- utilities ---

// static void strtolower(string& s) {
// 	for (auto& c : s)
// 		c = tolower(c);
// }

// static void chomp(string& s) {
// 	int  first = 0,  last = s.length();
// 	for (int i = 0; i < last; i++)
// 		if (!iswspace(s[i])) {
// 			first = i;
// 			break;
// 		}
// 	for (int i = last-1; i > first; i--)
// 		if (!iswspace(s[i])) {
// 			last = i+1;
// 			break;
// 		}
// 	s = s.substr(first, last-first);
// }

// true if string begins with '#'
static int is_meta(const string& s) {
	for (auto* cs = s.c_str(); *cs != '\0'; cs++) {
		if (iswspace(*cs))  continue;
		if (*cs == '#')     return 1;
		break;
	}
	return 0;
}

// short hash
static string strhash(const string& s) {
	static hash<string> mhash;
	static char cs[10];
	int i = mhash("s");
	snprintf(cs, 6, "%0x", i);
	return cs;
}

// exec and return result
static string exec(const string& cmd) {
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

// regex replace with callback, js style
static int regex_replace_cb(string& line, const regex& REG, void (*cb)(string& s)) {
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



//--- member funcs ---

static void label_cb(string& s) {
	if (s[1] < 'A' || s[1] > 'Z')
		s = mpattern + s.substr(1);
}


static int get_fnames(vector<string>& fnames, char type) {
	// all dasm16 files
	if (type == 'a') {
		string s;
		stringstream ss(exec("ls *.dasm16"));
		while (ss >> s) {
			if (s.length() >= 1 && s[0] == '_')  continue;  // ignore underscore files
			fnames.push_back(s);
		}
		if (fnames.size() == 0)
			fprintf(stderr, "no input files found.\n");
		return fnames.size();
	}
	// parse build files
	else if (type == 'b') {
		string s = "build.dasm16";
		ifstream ifs(s);
		if (!ifs.is_open()) {
			fprintf(stderr, "could not find build file: %s\n", s.c_str());
			return 0;
		}
		while (getline(ifs, s)) {
			smatch match;
			if (regex_match(s, match, REG_INCLUDE))
				fnames.push_back(match[1]);
		}
		return fnames.size();
	}
	// bad type
	fprintf(stderr, "unknown open type: %c\n", type);
	return 0;
}



//--- external funcs ---

int preproc() {
	// figure out files to compile
	vector<string> fnames;
	if (get_fnames(fnames, 'b') == 0)  return 1;
	// show files
	printf("compiling:\n");
	for (const auto& f : fnames)
		printf("    [%s]\n", f.c_str());
	printf("output:\n");
	printf("    [%s]\n", OUTPUT_DEF.c_str());
	// do the work
	ofstream ofs(OUTPUT_DEF);
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
			if (is_meta(s)) {
				; // parse_meta(s);  // save meta-command
				ofs << s << endl;  // output
			} else {
				regex_replace_cb(s, REG_LABEL, label_cb);  // mangle
				ofs << s << endl;  // output
			}
		}
	}
	return 0;
}


int main(int argc, char** argv) {
	return preproc();
}