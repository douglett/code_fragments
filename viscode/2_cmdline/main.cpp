#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <cassert>

using namespace std;


// consts
const string PROG_FNAME="test.cprog";
// vars
struct M_Struct {
	string name;
	vector<string> members;
};
vector<M_Struct> mstructs;
vector<string> argl;


int prog_load() {
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
				mstructs.back().members.push_back( s.substr(2) );
			}
			else if (s.substr(0,1)=="+") {
				mstructs.push_back({ .name=s.substr(1) });
			}
		}
	}
	return 0;
}

int prog_save() {
	string s;
	fstream fs(PROG_FNAME.c_str(), fstream::out);
	if (!fs.is_open())
		return fprintf(stderr, "error: could not save prog file (%s)\n", PROG_FNAME.c_str()), 1;
	fs << "[struct]" << endl;
	for (const auto& ms : mstructs) {
		fs << "+" << ms.name << endl;
		for (const auto& t : ms.members)
			fs << "++" << t << endl;
	}
	fs << "[func]" << endl;
	fs << "+name" << endl;
	fs << "++arg1" << endl;
	fs << "++arg2" << endl;
	return 0;
}


// get position using struct name
int get_struct_pos(const string& n) {
	for (int i=0; i<mstructs.size(); i++)
		if (mstructs[i].name == n)  return i;
	return -1;
}

// modify struct members
int cmd_struct_members() {
	string name = argl[2];
	int pos = get_struct_pos(name);
	if (pos == -1)
		return fprintf(stderr, "error: could not find struct: %s\n", name.c_str()), 1;
	else if (argl.size() == 3) {
		// show all members
		printf("struct [%s]:\n", name.c_str());
		for (const auto& m : mstructs[pos].members)
			printf("  %s\n", m.c_str());
	}
	return 0;
}

int cmd_struct() {
	if (argl.size()==2) {
		// show all structs
		printf("structs:\n");
		for (const auto& ms : mstructs)
			printf("  %s\n", ms.name.c_str());
	}
	else if (argl[2]=="-add") {
		// add new member
		if (argl.size()<4)
			return fprintf(stderr, "error: expected: struct add <name>\n"), 1;
		mstructs.push_back({ .name=argl[3] });
		printf("added struct: %s\n", argl[3].c_str());
		return prog_save();
	}
	else {
		// modify member
		return cmd_struct_members();
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
	if (prog_load())  return 1;
	// commands
	if (argl.size()<=1)
		return printf("commands: struct, func\n"), 0;
	else if (argl[1]=="struct") 
		return cmd_struct();
	// OK
	return 0;
}