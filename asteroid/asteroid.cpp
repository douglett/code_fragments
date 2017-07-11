// #include "globals.h"
#include "graphics.h"
#include <SDL.h>
#include <iostream>
#include <cassert>
#include <cmath>

using namespace std;


int makesprites();
int paintspinner();
SDL_Surface* screen = NULL;
SDL_Surface* ship = NULL;
SDL_Surface* spinner = NULL;
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
	SDL_Rect r;
	while (doloop) {
		// movement
		pos.x = pos.x + pos.r - pos.l;
		pos.y = pos.y + pos.d - pos.u;

		// draw
		SDL_FillRect(screen, NULL, 0x000000ff);
		r.x=pos.x,  r.y=pos.y;
		SDL_BlitSurface(ship, NULL, screen, &r);
		r.x=320-51,  r.y=240-51;
		paintspinner();
		SDL_BlitSurface(spinner, NULL, screen, &r);
		gfx::scale2x(screen, screen);
		gfx::flip();

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
	ship = gfx::mksprite(50, 50);
	SDL_FillRect(ship, NULL, 0xffffffff);
	// uint32_t* sp = (uint32_t*)ship->pixels;
	// for (int i=0; i<100; i++)
	// 	sp[ship->w * i + i] = 0xff0000ff;

	gfx::drawc(0xff0000ff);
	gfx::drawline(ship, 10, 10, 30, 40);
	gfx::drawc(0x0000ffff);
	gfx::drawline(ship, 10, 10, 49, 40);
	gfx::drawc(0x00ffffff);
	gfx::drawline(ship, 10, 45, 30, 10);
	gfx::drawc(0x999900ff);
	gfx::drawline(ship, 10, 40, 49, 30);

	spinner = gfx::mksprite(50, 50);
	SDL_FillRect(spinner, NULL, 0xffffffff);

	return 0;
}


int paintspinner() {
	static const double PI = 3.14159265;
	static int rot = 0;
	rot = (rot + 5) % 360;
	SDL_FillRect(spinner, NULL, 0xffffffff);
	int x = cos(rot * PI/180) * 20;
	int y = sin(rot * PI/180) * 20;
	
	gfx::drawc(0xff0000ff);
	gfx::drawline(spinner, 25-x, 25-y, 25+x, 25+y);
	gfx::drawpx(spinner, 25-x, 25-y);
	gfx::drawpx(spinner, 25+x, 25+y);

	gfx::drawc(0x00ff00ff);
	gfx::bresenham(spinner, 25-x, 25-y, 25+x, 25+y);

	return 0;
}