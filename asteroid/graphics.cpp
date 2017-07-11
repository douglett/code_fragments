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
	col = color;  // set global draw color
	return 0;
}
int gfx::drawpx(SDL_Surface* dst, int x, int y) {
	uint32_t* px = (uint32_t*)dst->pixels;  // get pixels
	if (y >= 0 && y < dst->h && x >= 0 && x < dst->w)  // bounds check
		px[dst->w * y + x] = col;  // set
	return 0;  // OK
}
int gfx::drawline(SDL_Surface* dst, int x1, int y1, int x2, int y2) {
	// calculate line info
	int ym = (y1 + y2) / 2;  // mid-point
	int xm = (x1 + x2) / 2;
	int dy = abs(y2 - y1);  // diff
	int dx = abs(x2 - x1);
	int mody = ( y2-y1 < 0 ? -1 : 1 );  // mod direction
	int modx = ( x2-x1 < 0 ? -1 : 1 );
	// y dominant
	if (dy > dx) {
		for (int i=0; i<dy/2; i++) {
			int y = i * mody;  // current y position
			int x = (dx / double(dy)) * i * modx;  // find x position from y position
			gfx::drawpx(dst, xm+x, ym+y);  // draw forwards from mid point
			gfx::drawpx(dst, xm-x, ym-y);  // draw backwards from mid point
		}
	}
	// x dominant
	else {
		for (int i=0; i<dx/2; i++) {
			int x = i * modx;  // current x position
			int y = (dy / double(dx)) * i * mody;  // find y position from x position
			gfx::drawpx(dst, xm+x, ym+y);  // draw forwards from mid point
			gfx::drawpx(dst, xm-x, ym-y);  // draw backwards from mid point
		}
	}
	return 0;  // OK
}