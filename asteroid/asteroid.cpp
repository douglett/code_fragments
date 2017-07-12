#include "globals.h"
#include "graphics.h"
#include <SDL.h>
#include <iostream>
#include <cassert>
#include <cmath>
#include <vector>
#include <array>

using namespace std;


int makesprites();
int paintspinner();


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


class WireFrame {
public:
	int x=0, y=0, r=0, s=100;
	vector<array<i32, 2>> points;
	
	int draw() {
		r %= 360;  // just in case
		double t = r * M_PI/180.0;  // theta (radians)
		double scale = s / 100.0;  // scale 1:xx
		// paint
		// x' = x cos⁡ θ − y sin⁡ θ
		// y' = y cos θ + x sin θ
		gfx::drawc(0xffffffff);
		for (int i=0; i<points.size()-1; i++) {
			auto& p1 = points[i];
			auto& p2 = points[i+1];
			gfx::drawline(SDL_GetVideoSurface(),
				x + ((p1[0]*cos(t) - p1[1]*sin(t)) * scale),
				y + ((p1[1]*cos(t) + p1[0]*sin(t)) * scale),
				x + ((p2[0]*cos(t) - p2[1]*sin(t)) * scale),
				y + ((p2[1]*cos(t) + p2[0]*sin(t)) * scale)
			);
		}
		// mid point
		// gfx::drawc(0xff0000ff);
		gfx::drawc( SDL_MapRGB(SDL_GetVideoSurface()->format, 0xff, 0, 0) );
		gfx::drawpx(SDL_GetVideoSurface(), x, y);
		return 0;
	}
};


SDL_Surface* screen = NULL;
SDL_Surface* ship = NULL;
SDL_Surface* spinner = NULL;

vector<WireFrame> wireframes;


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

	wireframes.emplace_back();
	wireframes.back().x = 100;
	wireframes.back().y = 100;
	wireframes.back().points = { {{-10,-10}}, {{0,-20}}, {{10,-10}}, {{10,10}}, {{-10,10}}, {{-10,-10}} };

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

		for (auto& wf : wireframes) {
			wf.r += 5;
			// wf.s += 1;
			wf.draw();
		}

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