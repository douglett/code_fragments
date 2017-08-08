#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <cassert>

using namespace std;


const string PROG_FNAME="test.cprog";
vector<string> argl;
struct M_Struct {
	string name;
	vector<string> types;
};
vector<M_Struct> mstructs;


int load() {
	string s;
	fstream fs(PROG_FNAME.c_str(), fstream::in);
	if (!fs.is_open())
		return fprintf(stderr, "error: missing prog file (%s)\n", PROG_FNAME.c_str()), 1;
	int flag=0;
	while (getline(fs, s)) {
		if      (s=="[struct]")  flag=1;
		else if (s=="[func]")  flag=2;
		else if (flag==1) {
			// load all structs
			if (s.substr(0,2)=="++") {
				assert(mstructs.size()>0);
				mstructs.back().types.push_back( s.substr(2) );
			}
			else if (s.substr(0,1)=="+") {
				mstructs.push_back({ .name=s.substr(1) });
			}
		}
	}
	return 0;
}


int cmd_struct() {
	if (argl.size()==2) {
		printf("structs:\n");
		for (const auto& ms : mstructs)
			printf("  %s\n", ms.name.c_str());
	}
	else if (argl[2]=="add") {
		if (argl.size()<4)
			return fprintf(stderr, "error: expected: struct add <name>\n"), 1;
		printf("added struct: %s\n", argl[3].c_str());
	}
	return 0;
}


int main(int argc, char** argv) {
	for (int i=0; i<argc; i++)
		argl.push_back(argv[i]);
	// show args
	printf("args: ");
	for (auto& a : argl)
		printf("[%s] ", a.c_str());
	printf("\n");
	// try and load prog
	if (load())  return 1;
	// commands
	if (argl.size()<=1)
		return printf("commands: struct, func\n"), 0;
	else if (argl[1]=="struct") 
		return cmd_struct();
	// OK
	return 0;
}