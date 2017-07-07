#include "graphics.h"
#include <SDL.h>
#include <iostream>
#include <cstdint>
#include <string>
#include <vector>

using namespace std;
// using namespace gfx;


SDL_Surface* gfx::mksprite(int w, int h) {
	SDL_Surface* s = SDL_CreateRGBSurface(SDL_SWSURFACE, w, h,
		32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
	SDL_FillRect(s, NULL, 0x00000000);
	return s;
}
int gfx::rmsprite(SDL_Surface* s) {
	SDL_FreeSurface(s);
	return 0;
}
int gfx::rmall() {
	return 0;
}


int gfx::scale2x(SDL_Surface* src, SDL_Surface* dst) {
	// lock
	SDL_LockSurface(src);
	// SDL_LockSurface(dst);
	// get some data
	uint32_t* sp = (uint32_t*)src->pixels;
	// uint32_t* dp = (uint32_t*)dst->pixels;
	int ww = min(src->w, dst->w/2);
	int hh = min(src->h, dst->h/2);
	// loop
	uint32_t c;
	SDL_Rect r={ 0, 0, 2, 2 };
	for (int y=hh-1; y>=0; y--)
	for (int x=ww-1; x>=0; x--) {
		c = sp[src->w * y + x];
		r.x = x*2,  r.y = y*2;
		SDL_FillRect(dst, &r, c);
	}
	// unlock
	// SDL_UnlockSurface(dst);
	SDL_UnlockSurface(src);
	return 0;
}


int gfx::flip() {
	SDL_Flip(SDL_GetVideoSurface());
	SDL_Delay(16);
	return 0;
}