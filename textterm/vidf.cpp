#include "globals.h"
#include <iostream>
#include <fstream>
#include <sys/stat.h>

using namespace std;


static const int C_MAX=1024;
static char c_str[C_MAX];
#define  strfmt(...)  (snprintf(c_str, C_MAX, __VA_ARGS__),  c_str)

static int gett(const string& fname) {
	struct stat st;
	if (stat(fname.c_str(), &st))  return -1;
	return st.st_mtime;
}

struct Vfile {
	string fname;
	int mtime;
	int x, y, z;
};
static vector<Vfile> flist;


int vidf::init(const string& s) {
	flist={};
	string conf=s+"/vid.conf";
	int tt=gett(conf);
	if (tt==-1) {
		fprintf(stderr, "error: config file not found: %s\n", conf.c_str());
		return 1;
	}
	flist.push_back({ conf, 0 });
	return 0;
}
int vidf::update() {
	int tt=gett(flist[0].fname);
	if (tt > flist[0].mtime) {
		// load conf
		fstream fs(flist[0].fname, fstream::in);
		string s;
		int x,y,z;
		while (!fs.eof()) {
			fs >> s>>x>>y>>z;
			printf("[%02d] %s  %d %d %d\n", s.c_str(), x,y,z);
		}
		// update modtime
		printf("%d %d  %d\n", flist[0].mtime, tt, tt-flist[0].mtime);
		flist[0].mtime=tt;
	}
	return 0;
}