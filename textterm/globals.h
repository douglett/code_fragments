#pragma once

#include <SDL.h>
#include <vector>
#include <string>

namespace vid {
	const int SURFACE_MAX=1024;
	// const int SCREEN_W=320, SCREEN_H=240;
	extern SDL_Surface           *screen, *vmem;
	extern std::vector<uint32_t> keylist;
	extern uint32_t              tcolor;
	void         init();
	void         quit();
	SDL_Surface* makesurface(int w, int h);
	void         scaleto(SDL_Surface* src, SDL_Surface* dst);
	int          flipvid();
} // end vid

namespace term {
	extern std::vector<std::string> texthist;
	
} // end term