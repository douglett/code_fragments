#include <iostream>
#include <string>
#include <vector>
#include <fstream>

using namespace std;


vector<string> split(const string& s) {
	vector<string> vs={ "" };
	for (char c : s)
		if (isspace(c))  { if (vs.back().length()) vs.push_back(""); }
		else  { vs.back() += c; }
	if (vs.back().length()==0)  vs.pop_back();
	return vs;
}


int namespaces() {
	string fname="test.proj";
	fstream fs(fname, fstream::in);
	if (!fs.is_open())
		return fprintf(stderr, "error: could not open %s\n", fname.c_str());
	printf("namespaces:  ");
	string s;
	while (fs >> s)
		printf("%s ", s.c_str());
	printf("\n");
	return 0;
}


int main() {
	string s;
	while (printf(">> ") && getline(cin, s)) {
		// printf("[%s]\n", s.c_str());
		auto vs = split(s);
		// show
		for (const auto& s : vs)
			printf("[%s] ", s.c_str());
		printf("\n");
		// commands
		if      (vs.size()==0) ;
		else if (vs[0]=="ns")  namespaces();
	}
}