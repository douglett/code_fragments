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

namespace keys {
	// stuff
	int u=0, d=0, l=0, r=0;
	int x=20, y=20;

	int update() {
		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			int keydir=0,  showkey=0;
			switch (e.type) {
			case SDL_QUIT:  return -1;  // quit signal
			case SDL_KEYDOWN:
			case SDL_KEYUP:
				keydir = (e.type==SDL_KEYDOWN ? 1 : -1);
				switch (e.key.keysym.sym) {
				case SDLK_ESCAPE:  return -1;  // quit signal
				case SDLK_LEFT:    if (showkey) printf("l %d\n", keydir);  l = (keydir==1 ? 1 : 0);  break;
				case SDLK_RIGHT:   if (showkey) printf("r %d\n", keydir);  r = (keydir==1 ? 1 : 0);  break;
				case SDLK_UP:      if (showkey) printf("u %d\n", keydir);  u = (keydir==1 ? 1 : 0);  break;
				case SDLK_DOWN:    if (showkey) printf("d %d\n", keydir);  d = (keydir==1 ? 1 : 0);  break;
				default:  printf("%d\n", e.key.keysym.sym);
				}
				break;
			}
		}
		return 0;
	}
}


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
	while (doloop) {
		// movement
		keys::x = keys::x + keys::r - keys::l;
		keys::y = keys::y + keys::d - keys::u;

		// draw
		SDL_FillRect(screen, NULL, 0x000000ff);

		// SDL_Rect r;
		// r.x=pos.x,  r.y=pos.y;
		// SDL_BlitSurface(ship, NULL, screen, &r);
		// r.x=320-51,  r.y=240-51;
		// paintspinner();
		// SDL_BlitSurface(spinner, NULL, screen, &r);

		if (keys::update()==-1)  doloop=0;

		gfx::scale2x(screen, screen);
		gfx::flip();		
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
	
	gfx::drawc(0x0000ffff);
	gfx::drawpx(spinner, 25-x, 25-y);
	gfx::drawpx(spinner, 25+x, 25+y);

	return 0;
}