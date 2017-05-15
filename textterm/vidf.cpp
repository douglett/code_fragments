#include "globals.h"
#include <iostream>
#include <fstream>
#include <sys/stat.h>

using namespace std;


// private classes
struct Vfile        {  string fname;  int mtime=0;  };
struct Vtxt : Vfile {  fstream fs;  };
struct Vbmp : Vfile {  int id=-1, x=0, y=0, z=0;  SDL_Surface* sf=NULL;  };
// private vars
static string base_path;
static Vtxt   conffile;
static std::vector<Vbmp> flist;
// private functions
static int gett(const string& fname) {
	struct stat st;
	if (stat(fname.c_str(), &st))  return -1;
	return st.st_mtime;
}
static int update_surface(int id, int x, int y, int z) {
	for (auto& v : flist)
	if (v.id==id) {
		v.x=x,  v.y=y,  v.z=z;
		// printf("[%03d %03d %03d %03d]\n", id, x, y, z);
		return 1;
	}
	return 0;
}
static int update_conf() {
	// update
	char s[4]   = {0};
	int  num[4] = {0};
	conffile.fs.clear();
	for (int i=0; i<64; i++) {
		for (int j=0; j<4; j++) {
			conffile.fs.seekp(i*4*4 + j*4);
			conffile.fs.get(s, 4);
			num[j] = atoi(s);
		}
		// if ((int)conffile.fs.tellp()==-1)  break;
		if (conffile.fs.eof())  break;
		// printf("[%03d %03d %03d %03d]\n", num[0], num[1], num[2], num[3]);
		update_surface(num[0], num[1], num[2], num[3]);
	}
	return 0;
}
static int update_surface(Vbmp& f) {
	int tt = gett(base_path+f.fname);
	if (tt <= f.mtime)  return 0;
	f.mtime = tt;
	SDL_FreeSurface(f.sf);
	f.sf = vid::loadsurface(base_path+f.fname);
	printf("update: %s %d\n", f.fname.c_str(), f.mtime);
	return 1;
}


namespace vidf {
	// public functions
	int init(const string& path) {
		flist = {};
		base_path = path;
		if (base_path.size() && base_path.back()!='/')  base_path+='/';  // add trailing slash
		// load config file		
		conffile.fname = "vid.conf",  conffile.mtime = 0;  // reset conf
		string vpath = base_path + conffile.fname;  // get base path
		conffile.mtime = gett(vpath);  // update time
		if (conffile.mtime == -1) {
			fprintf(stderr, "error: config file not found: %s\n", vpath.c_str());
			return 1;
		}
		conffile.fs.open(vpath, fstream::in | fstream::out);  // (re)open file
		// update 64 bmp files
		for (int i=0; i<64; i++) {
			string fn = strfmt("%02d.bmp", i);
			int tt = gett(base_path+fn);
			if (tt == -1)  continue;
			// flist.push_back(fn);
			flist.emplace_back();
			flist.back().fname = fn;
			flist.back().id = i;
		}
		// update_conf();
		return 0;
	}
	void quit() {
		for (auto& f : flist)  SDL_FreeSurface(f.sf);
		flist = { };
		conffile.mtime = 0;
	}
	int is_init() {
		return ( conffile.mtime > 0 );
	}
	int update() {
		update_conf();
		for (auto& f : flist)  update_surface(f);
		return 0;
	}
	int flip() {
		// display file vram
		update();
		SDL_FillRect(vid::screen, NULL, SDL_MapRGB(vid::screen->format, 0,0,0));
		SDL_Rect r={0,0};
		for (auto& f : flist) {
			r.x=f.x,  r.y=f.y;
			SDL_BlitSurface(f.sf, NULL, vid::screen, &r);
		}
		return 0;
	}
} // end vidf