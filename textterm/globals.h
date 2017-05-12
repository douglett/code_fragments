#pragma once

#include <SDL.h>
#include <vector>
#include <string>
#include <array>

namespace vid {
	const int SURFACE_MAX=1024;
	// const int SCREEN_W=320, SCREEN_H=240;
	extern SDL_Surface           *screen, *vmem;
	extern std::vector<std::array<int,6>> sprlist;
	extern std::vector<uint32_t> keylist;
	extern uint32_t              tcolor;
	void         init();
	void         quit();
	SDL_Surface* makesurface(int w, int h);
	SDL_Surface* loadsurface(const std::string& name);
	void         scaleto(SDL_Surface* src, SDL_Surface* dst);
	int          flipvid();
} // end vid

namespace vidf {
	struct Vfile {
		std::string fname;
		int mtime=0, x=0, y=0, z=0;
		SDL_Surface* sf=NULL;
	};
	extern std::vector<Vfile> flist;
	int  init   (const std::string& path);
	int  update ();
} // end vidf

namespace term {
	extern std::vector<std::string> texthist;
	// extern std::string textpg;
	// std::string& gettextpg();
} // end term