#include "graphics.h"
#include <SDL.h>
#include <iostream>
#include <cstdint>
#include <string>
#include <vector>
#include <cmath>

using namespace std;


static uint32_t col = 0;
SDL_Surface* gfx::font = NULL;


int gfx::init(int w, int h, std::string winname) {
	if (sizeof(int) != sizeof(int32_t)) {
		fprintf(stderr, "error: expected 32 bit standard int\n");
		if (gfx::ERROR_EXIT)  exit(1);
		return 1;
	}
	int err = SDL_Init(SDL_INIT_VIDEO);
	if (err!=0) {
		fprintf(stderr, "error loading SDL: %d\n", err);
		if (gfx::ERROR_EXIT)  exit(1);
		return err;
	}
	SDL_Surface* screen = SDL_SetVideoMode(w, h, 32, SDL_HWSURFACE);
	if (screen==NULL) {
		fprintf(stderr, "error: could not load video\n");
		if (gfx::ERROR_EXIT)  exit(1);
		return 1;
	}
	SDL_WM_SetCaption(winname.c_str(), winname.c_str());
	SDL_SetColorKey(screen, SDL_SRCCOLORKEY, gfx::drawc(255,0,255));
	printf("gfx::init: init OK: w:%d h:%d  %s\n", w, h, winname.c_str());
	gfx::mkfont();  // make font
	gfx::drawc(255,255,255);  // set default draw color to white
	srand( time(NULL) );  // seed timer
	return 0;
}


SDL_Surface* gfx::mksprite(int w, int h) {
	SDL_PixelFormat* fmt = SDL_GetVideoSurface()->format;
	SDL_Surface* sf = SDL_CreateRGBSurface(SDL_SWSURFACE, w, h,
		fmt->BitsPerPixel, fmt->Rmask, fmt->Gmask, fmt->Bmask, fmt->Amask);
	SDL_FillRect(sf, NULL, gfx::drawc(0,0,0));
	SDL_SetColorKey(sf, SDL_SRCCOLORKEY, gfx::drawc(255,0,255));
	return sf;
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
	// clear events before flip
	SDL_Event e;
	while (SDL_PollEvent(&e))
		if (e.type == SDL_QUIT) {
			fprintf(stderr, "error: gfx::flip: untrapped window quit event\n");
			exit(1);
		}
	// flip
	SDL_Flip(SDL_GetVideoSurface());
	SDL_Delay(16);
	return 0;  // flip OK
}


uint32_t gfx::drawc(uint32_t color) {
	col = color;  // set global draw color
	return col;
}
uint32_t gfx::drawc(uint8_t r, uint8_t g, uint8_t b) {
	col = SDL_MapRGB(SDL_GetVideoSurface()->format, r, g, b);
	return col;
}
int gfx::drawpx(SDL_Surface* dst, int x, int y) {
	uint32_t* px = (uint32_t*)dst->pixels;  // get pixels
	if (y >= 0 && y < dst->h && x >= 0 && x < dst->w)  // bounds check
		px[dst->w * y + x] = col;  // set
	return 0;  // OK
}
int gfx::drawline(SDL_Surface* dst, int x1, int y1, int x2, int y2) {
	// Bresenham's line algorithm
	// init values
	const int steep = (abs(y2 - y1) > abs(x2 - x1));
	if (steep)    swap(x1, y1),  swap(x2, y2);
	if (x1 > x2)  swap(x1, x2),  swap(y1, y2);
	const double dx = x2 - x1;
	const double dy = abs(y2 - y1);
	const int ystep = ( y1 < y2 ? 1 : -1 );
	double error = dx / 2.0f;
	// do loop
	int y = y1;
	for (int x=x1; x<x2; x++) {
		if (steep)  gfx::drawpx(dst, y, x);
		else        gfx::drawpx(dst, x, y);
		error -= dy;
		if (error < 0)  y += ystep,  error += dx;
	}
	if (steep)  gfx::drawpx(dst, y2, x2);  // do final pixel
	else        gfx::drawpx(dst, x2, y2);
	return 0;  // OK
}