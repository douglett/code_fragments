// #include "globals.h"
#include "graphics.h"
#include <SDL.h>
#include <iostream>
#include <cassert>

using namespace std;


int makesprites();
int scale2x(SDL_Surface* src, SDL_Surface* dst);
SDL_Surface* screen = NULL;
SDL_Surface* ship = NULL;
struct pos_t {
	int u=0, d=0, l=0, r=0;
	int x=20, y=20;
};
pos_t pos;


int main(int argc, char** argv) {
	printf("start\n");
	assert(sizeof(int)==sizeof(int32_t));
	int err = SDL_Init(SDL_INIT_VIDEO);
	if (err!=0) {
		fprintf(stderr, "error loading SDL: %d\n", err);
		return 1;
	}
	screen = SDL_SetVideoMode(640, 480, 32, SDL_HWSURFACE);
	if (screen==NULL) {
		fprintf(stderr, "error: could not load video\n");
		return 1;
	}

	makesprites();

	int doloop=1;
	SDL_Event e;
	while (doloop) {
		// movement
		pos.x = pos.x + pos.r - pos.l;
		pos.y = pos.y + pos.d - pos.u;

		// draw
		SDL_FillRect(screen, NULL, 0x000000ff);
		SDL_Rect r={ Sint16(pos.x), Sint16(pos.y) };
		SDL_BlitSurface(ship, NULL, screen, &r);
		scale2x(screen, screen);
		SDL_Flip(screen);
		SDL_Delay(16);

		while (SDL_PollEvent(&e)) {
			int keydir=0,  showkey=0;
			switch (e.type) {
			case SDL_QUIT:  doloop=0;  break;
			case SDL_KEYDOWN:
			case SDL_KEYUP:
				keydir = (e.type==SDL_KEYDOWN ? 1 : -1);
				switch (e.key.keysym.sym) {
				case SDLK_ESCAPE:  doloop=0;  break;
				case SDLK_LEFT:    if (showkey) printf("l %d\n", keydir);  pos.l = (keydir==1 ? 1 : 0);  break;
				case SDLK_RIGHT:   if (showkey) printf("r %d\n", keydir);  pos.r = (keydir==1 ? 1 : 0);  break;
				case SDLK_UP:      if (showkey) printf("u %d\n", keydir);  pos.u = (keydir==1 ? 1 : 0);  break;
				case SDLK_DOWN:    if (showkey) printf("d %d\n", keydir);  pos.d = (keydir==1 ? 1 : 0);  break;
				default:  printf("%d\n", e.key.keysym.sym);
				}
				break;
			}
		}
	}

	SDL_Quit();
	return 0;
}


int makesprites() {
	ship = SDL_CreateRGBSurface(SDL_SWSURFACE, 32, 32, 32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
	SDL_FillRect(ship, NULL, 0xffffffff);
	uint32_t* sp = (uint32_t*)ship->pixels;
	for (int i=0; i<100; i++)
		sp[ship->w * i + i] = 0xff0000ff;
	return 0;
}


int scale2x(SDL_Surface* src, SDL_Surface* dst) {
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