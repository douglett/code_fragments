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
	int action=0;

	int update() {
		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			int keydir=0,  showkey=1;
			switch (e.type) {
			case SDL_QUIT:  return -1;  // quit signal
			case SDL_KEYDOWN:
			case SDL_KEYUP:
				keydir = (e.type==SDL_KEYDOWN ? 1 : 0);
				switch (e.key.keysym.sym) {
				case SDLK_ESCAPE:  return -1;  // quit signal
				case SDLK_LEFT:    if (showkey) printf("l %d\n", keydir);  l = keydir;  break;
				case SDLK_RIGHT:   if (showkey) printf("r %d\n", keydir);  r = keydir;  break;
				case SDLK_UP:      if (showkey) printf("u %d\n", keydir);  u = keydir;  break;
				case SDLK_DOWN:    if (showkey) printf("d %d\n", keydir);  d = keydir;  break;
				case SDLK_SPACE:   if (showkey) printf("SPC %d\n", keydir);  action = keydir;  break;
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
	static const int MID_POINT=1;
	static const int HIT_DISPLAY=1;

	double x=0, y=0, rotate=0, scale=100;
	double speed=0, torque=0, drift=0, drifttorque=0;
	double hit_distance=0;
	vector<array<i32, 2>> points;
	string id;
	int flags=0;
	
	int draw() {
		rotate = fmod(rotate, 360);  // constrain just in case
		drift  = fmod(drift, 360);  // constrain just in case
		double t = (rotate + drift) * M_PI/180.0;  // theta (radians)
		double mscale = scale / 100.0;  // scale: percentage to 1:xx
		// paint
		// x' = x cos⁡ θ − y sin⁡ θ
		// y' = y cos θ + x sin θ
		gfx::drawc(255,255,255);
		for (int i=0; i<points.size()-1; i++) {
			auto& p1 = points[i];
			auto& p2 = points[i+1];
			gfx::drawline(SDL_GetVideoSurface(),
				x + ((p1[0]*cos(t) - p1[1]*sin(t)) * mscale),
				y + ((p1[1]*cos(t) + p1[0]*sin(t)) * mscale),
				x + ((p2[0]*cos(t) - p2[1]*sin(t)) * mscale),
				y + ((p2[1]*cos(t) + p2[0]*sin(t)) * mscale)
			);
		}
		// mid point
		// gfx::drawc(0xff0000ff);
		gfx::drawc(255, 0, 0);
		if (MID_POINT)  gfx::drawpx(SDL_GetVideoSurface(), x, y);
		if (HIT_DISPLAY)  
			gfx::drawline(SDL_GetVideoSurface(), x, y, x+hit_distance, y),
			gfx::drawline(SDL_GetVideoSurface(), x, y, x, y+hit_distance);
		return 0;
	}

	int step() {
		x      += sin(rotate * M_PI/180) * speed;
		y      -= cos(rotate * M_PI/180) * speed;  // cosine moves in negative (up) direction
		rotate += torque;
		drift  += drifttorque;
		return 0;
	}
};


SDL_Surface* screen = NULL;
SDL_Surface* ship = NULL;
SDL_Surface* spinner = NULL;

vector<WireFrame> wireframes;


int make_objects() {
	WireFrame obj;
	// spaceship
	obj.x = 100,  obj.y = 100;
	obj.hit_distance = 10;
	obj.points = { {{-10,-7}}, {{0,-15}}, {{10,-7}}, {{10,10}}, {{-10,10}}, {{-10,-7}} };
	obj.id = "spaceship";
	wireframes.push_back(obj);
	// asteroid
	obj.x = 200,  obj.y = 100;
	obj.hit_distance = 15;
	obj.points = { {{-5,-16}}, {{0,-18}}, {{12,-13}}, {{15,-4}}, {{9,12}}, {{0,16}}, {{-16,0}}, {{-5,-16}} };
	obj.id = "asteroid";
	obj.rotate = 30,  obj.speed = 1,  obj.drifttorque = 3;
	wireframes.push_back(obj);
	return 0;
}


int make_laser() {
	printf("make laser\n");
	WireFrame obj;
	obj.points = { {{0,-4}}, {{0,4}} };
	obj.id = "laser";
	obj.hit_distance = 3;
	obj.x = wireframes[0].x,  obj.y = wireframes[0].y,  obj.rotate = wireframes[0].rotate;
	obj.speed = 5;
	wireframes.push_back(obj);
	return 0;
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
	SDL_WM_SetCaption("asteroids", "asteroids");

	// make wireframes
	make_objects();

	int doloop=1;
	int action=0;
	while (doloop) {
		// movement
		wireframes[0].speed  = (keys::u - keys::d) * 3;
		wireframes[0].torque = (keys::r - keys::l) * 5;
		if (keys::action && !action)  action = 1,  make_laser();
		if (!keys::action && action)  action = 0;

		// redraw
		SDL_FillRect(screen, NULL, gfx::drawc(0,0,0));
		for (auto& wf : wireframes) {
			wf.step();
			int offdir = 0;
			if (wf.x < -10)  wf.x = 320+20 + wf.x,  offdir = 1;
			if (wf.y < -10)  wf.y = 240+20 + wf.y,  offdir = 2;
			if (wf.x > 330)  wf.x = -10 + fmod(wf.x, 330),  offdir = 3;
			if (wf.y > 250)  wf.y = -10 + fmod(wf.y, 250),  offdir = 4;
			if (offdir && wf.id == "laser") {
				wf.flags = -1;
				continue;
			}
			wf.draw();
		}

		// erase dead
		for (int i=wireframes.size()-1; i>=0; i--)
			if (wireframes[i].flags==-1)
				wireframes.erase(wireframes.begin()+i);

		// flip and update
		gfx::scale2x(screen, screen);
		if (keys::update()==-1)  doloop=0;
		gfx::flip();		
	}

	SDL_Quit();
	return 0;
}
