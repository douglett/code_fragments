#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <map>

using namespace std;


struct Ndef {
	string id, type, construct;
};
struct Nstruct {
	string id;
	vector<string> body;
};
struct Nfunc {
	string id, rval;
	vector<string> args, head, body;
};

vector<string> nslist;
vector<Nstruct> structlist;
vector<Nfunc> funclist;


string chomp(const string& s) {
	int a=0, z=s.size()-1;
	for ( ; a<s.size() && isspace(s[a]); a++) ;
	for ( ; z>=0 && isspace(s[z]); z--) ;
	return s.substr(a, z+1-a);
}


int load() {
	fstream fs("def.txt", fstream::in);
	if (!fs.is_open())
		return fprintf(stderr, "error: missing file\n"), 1;
	int state=0;
	string s;
	while (getline(fs, s)) {
		s = chomp(s);
		// switch state
		if      (s.substr(0,10)=="namespace:") state=1, s=chomp(s.substr(10));
		else if (s.substr(0,7)=="struct:") state=2, s=chomp(s.substr(7));
		else if (s.substr(0,5)=="func:") state=3, s=chomp(s.substr(5));
		// printf("[%s]\n", s.c_str());
		// work on states
		if (state==1) {
			if (s.length())
				nslist.push_back(s);
		}
		else if (state==2) {
			if (s.substr(0,3)=="id:")
				structlist.push_back({ .id=chomp(s.substr(3)) });
			else if (s.substr(0,5)=="body:")
				state=21;
		}
		else if (state==21) {  // struct body
			if (s.length())
				structlist.back().body.push_back(s);
		}
		else if (state==3) {
			if (s.substr(0,3)=="id:")
				funclist.push_back({ .id=chomp(s.substr(3)) });
			else if (s.substr(0,5)=="rval:")
				funclist.back().rval=chomp(s.substr(5));
			else if (s.substr(0,5)=="args:")  state=31;
			else if (s.substr(0,5)=="head:")  state=32;
			else if (s.substr(0,5)=="body:")  state=33;
		}
		else if (state==31) {  // func args
			if      (s.substr(0,5)=="head:")  state=32;
			else if (s.substr(0,5)=="body:")  state=33;
			// funclist.back().args.push_back(s);
		}
		else if (state==32) {  // func head
			if (s.substr(0,5)=="body:")  state=33;
		}
		else if (state==33) {  // func body
			funclist.back().body.push_back(s);
		}
	}
	return 0;
}

int show() {
	printf("namespace:\n");
	for (const auto& n : nslist)
		printf("    %s\n", n.c_str());
	printf("\n");
	printf("struct:\n");
	for (const auto& n : structlist) {
		printf("    %s\n", n.id.c_str());
		for (const auto& n : n.body)
			printf("        %s\n", n.c_str());
	}
	return 0;
}

Ndef exdef(const string& def) {
	string id, type, cons;
	int i=0;
	for ( ; i<def.size() && def[i]!='('; i++)
		id+=def[i];
	for (i++ ; i<def.size() && def[i]!=')'; i++)
		type+=def[i];
	for (i++ ; i<def.size(); i++)
		cons+=def[i];
	return { .id=id, .type=type, .construct=cons };
}

int compile() {
	fstream fs("out.cpp.x", fstream::out);
	// header
	static const vector<string> DEFHEAD={ "iostream", "string", "vector", "cmath", "cstdint" };
	for (const auto& d : DEFHEAD)
		fs<<"#include <"<<d<<">"<<endl;
	fs<<endl<<"using namespace std;"<<endl<<endl;
	// structs
	for (const auto& s : structlist) {
		fs<<"struct "<<s.id<<" {"<<endl;
		for (const auto& b : s.body) {
			// fs<<'\t'<<exdef(b)<<";"<<endl;
			auto d=exdef(b);
			string s=d.type+" "+d.id+"="+d.construct;
			fs<<'\t'<<s<<";"<<endl;
		}
		fs<<"};"<<endl;
	}
	// functions
	for (const auto& f : funclist) {
		auto rval = exdef(f.rval);
		fs<<rval.type<<" "<<f.id<<"() {"<<endl;
		fs<<'\t'<<rval.type<<" rval="<<rval.construct<<";"<<endl;
		for (const auto& b : f.body)
			fs<<'\t'<<b<<endl;
		fs<<"\treturn rval;"<<endl;
		fs<<"}"<<endl;
	}
	return 0;
}


int main() {
	load();
	// show();
	compile();
}