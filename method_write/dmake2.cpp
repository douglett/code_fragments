#include <iostream>
#include <string>
#include <vector>
#include <fstream>

using namespace std;


const string spacer="\n/* ***** */\n\n";
vector<string> args;
vector<string> method_sigs;
vector<string> method_body;


string default_header();


int main(int argc, char** argv) {
	for (int i=0; i<argc; i++)
		args.push_back(argv[i]);

	fstream fs("test.c2", fstream::in);
	// fstream fs2("test.cpp", fstream::out);
	string ln;
	int state=0;
	while (getline(fs, ln)) {
		switch (state) {
		case 0:
			if (ln=="@method")  state=1;
			break;
		case 1:
			method_sigs.push_back(ln+";");
			method_body.push_back(ln+" {");
			state=2;
			break;
		case 2:
			if (ln=="@method")  { method_body.push_back("}");  method_body.push_back("");  state=1; }
			break;
		}
	}
	if (state==2)  { method_body.push_back("}");  state=0; }

	cout << default_header();
	cout << spacer;
	for (const auto& m : method_sigs)
		cout << m << endl;
	cout << spacer;
	for (const auto& m : method_body)
		cout << m << endl;
}


string default_header() {
	static const vector<string> includes={
		"iostream", "cstdint", "cassert", "string", "vector", "map", "fstream", "sstream" };
	string s;
	for (const auto& inc : includes)
		s+="#include <"+inc+">\n";
	s+="\n";
	s+="typedef i32 int32_t;\n";
	s+="typedef u32 uint32_t;\n";
	s+="\nusing namespace std\n";
	return s;
}