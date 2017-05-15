#pragma once

#include <SDL.h>
#include <string>
#include <vector>
#include <array>

namespace vid {
	const int SURFACE_MAX=1024;
	// const int SCREEN_W=320, SCREEN_H=240;
	extern SDL_Surface           *screen, *qbfont;
	extern uint32_t              tcolor, fgcolor;
	extern std::vector<uint32_t> keylist;
	void         init();
	void         quit();
	SDL_Surface* makesurface(int w, int h);
	SDL_Surface* loadsurface(const std::string& name);
	void         scaleto(SDL_Surface* src, SDL_Surface* dst);
	int          vmode(int mode=0);
	int          flipvid();
} // end vid

namespace vram {
	extern SDL_Surface *vmem;
	extern std::vector<std::array<int,6>> sprlist;
	void init      ();
	void quit      ();
	int  is_init   ();
	int  flip      ();
	int  dbgflip   ();
	void testscene ();
} // end vram

namespace vidf {
	struct Vfile {
		std::string fname;
		int mtime=0, x=0, y=0, z=0;
		SDL_Surface* sf=NULL;
	};
	extern std::vector<Vfile> flist;
	int  init    (const std::string& path);
	void quit    ();
	int  is_init ();
	int  update  ();
	int  flip    ();
} // end vidf

namespace term {
	const  int  C_MAX=1024;
	extern char c_str[C_MAX];
	extern std::vector<std::string> texthist;
	void log  (const std::string& s);
	int  flip ();
} // end term
#define  strfmt(...)  (snprintf(term::c_str, term::C_MAX, __VA_ARGS__),  term::c_str)
#define  logfmt(...)  term::log(strfmt(__VA_ARGS__))