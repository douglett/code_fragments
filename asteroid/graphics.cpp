#include "graphics.h"
#include <SDL.h>
#include <iostream>
#include <cstdint>
#include <string>
#include <vector>

using namespace std;


static uint32_t col = 0;


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


int gfx::drawc(uint32_t color) {
	col = color;
	return 0;
}


int gfx::drawline(SDL_Surface* dst, int x1, int y1, int x2, int y2) {
	uint32_t* px = (uint32_t*)dst->pixels;
	// for (int i=0; i<100; i++)
	// 	sp[ship->w * i + i] = 0xff0000ff;

	int dy = abs(y2 - y1);
	int dx = abs(x2 - x1);
	int mody = ( y2-y1 < 0 ? -1 : 1 );
	int modx = ( x2-x1 < 0 ? -1 : 1 );
	printf("%d %d\n", dy, dx);

	if (dy > dx) {
		for (int i=0; i<dy; i++) {
			int x = (dx / double(dy)) * i;
			if (y1+i>=0 && y1+i<dst->h && x1+x>=0 && x1+x<dst->w)
				px[dst->w * (y1+i) + x1+x] = col;

			// int x = x1 + ((dx / double(dy)) * i * modx);
			// int y = y1 + (i * mody);
		}
	}
	else {
		for (int i=0; i<dx; i++) {
			int y = (dy / double(dx)) * i;
			if (y1+y>=0 && y1+y<dst->h && x1+i>=0 && x1+i<dst->w)
				px[dst->w * (y1+y) + x1+i] = col;
		}
	}

	return 0;
}