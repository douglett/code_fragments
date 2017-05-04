#pragma once

#include <SDL.h>

namespace vid {
	const int SURFACE_MAX=1024;
	extern SDL_Surface* screen;
	extern Uint32       tcolor;
	void         init();
	SDL_Surface* makesurface(int w, int h);
} // end vid