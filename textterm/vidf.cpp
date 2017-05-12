#include "globals.h"
#include <iostream>
#include <fstream>
#include <sys/stat.h>

using namespace std;


// private consts
static const int C_MAX=1024;
// private vars
static char c_str[C_MAX];
static string base_path;
static vidf::Vfile fconf;
// private functions
#define  strfmt(...)  (snprintf(c_str, C_MAX, __VA_ARGS__),  c_str)
static int gett(const string& fname) {
	struct stat st;
	if (stat(fname.c_str(), &st))  return -1;
	return st.st_mtime;
}
static int update_surface(vidf::Vfile& f) {
	int tt=gett(base_path+f.fname);
	if (tt <= f.mtime)  return 0;
	f.mtime = tt;
	SDL_FreeSurface(f.sf);
	f.sf=vid::loadsurface(base_path+f.fname);
	printf("update: %s %d\n", f.fname.c_str(), f.mtime);
	return 1;
}


// public vars
std::vector<vidf::Vfile> vidf::flist;
// public functions
int vidf::init(const string& path) {
	flist={};
	base_path=path;
	if (base_path.size() && base_path.back()!='/')  base_path+='/';  // add trailing slash
	// load config file
	fconf.fname="vid.conf",  fconf.mtime=0;
	string conf=base_path+fconf.fname;
	int tt=gett(conf);
	if (tt==-1) {
		fprintf(stderr, "error: config file not found: %s\n", conf.c_str());
		return 1;
	}
	// update 64 bmp files
	for (int i=0; i<64; i++) {
		string fn=strfmt("%02d.bmp", i);
		int tt=gett(base_path+fn);
		if (tt==-1)  continue;
		// flist.push_back(fn);
		flist.emplace_back();
		flist.back().fname=fn;
	}
	return 0;
}
int vidf::update() {
	// fconf
	for (auto& f : flist)  update_surface(f);
	return 0;
}