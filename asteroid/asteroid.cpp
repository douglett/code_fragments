#include "globals.h"
#include "graphics.h"
#include <SDL.h>
#include <iostream>
#include <cassert>
#include <cmath>
#include <vector>
#include <array>

using namespace std;


namespace keys {
	// directions
	int u=0, d=0, l=0, r=0;

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
	double x=0, y=0, r=0, s=1;
	vector<array<i32, 2>> points;
	
	int draw() {
		r = fmod(r, 360);  // constrain just in case
		double t = r * M_PI/180.0;  // theta (radians)
		double scale = s;  // scale 1:xx
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

	// make wireframe
	wireframes.emplace_back();
	wireframes.back().x = 100;
	wireframes.back().y = 100;
	wireframes.back().points = { {{-10,-10}}, {{0,-20}}, {{10,-10}}, {{10,10}}, {{-10,10}}, {{-10,-10}} };

	int doloop=1;
	while (doloop) {
		// movement
		// wireframes[0].x += keys::r - keys::l;
		// wireframes[0].y += keys::d - keys::u;
		wireframes[0].r += (keys::r - keys::l) * 5;
		wireframes[0].x += cos((wireframes[0].r + 90) * M_PI/180) * (keys::d - keys::u)*5;
		wireframes[0].y += cos(wireframes[0].r * M_PI/180) * (keys::d - keys::u)*5;

		// redraw
		SDL_FillRect(screen, NULL, 0x000000ff);
		for (auto& wf : wireframes) {
			// wf.r += 5;
			// wf.s += 1;
			wf.draw();
		}

		// flip and update
		gfx::scale2x(screen, screen);
		if (keys::update()==-1)  doloop=0;
		gfx::flip();		
	}

	SDL_Quit();
	return 0;
}