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


SDL_Surface *screen = NULL,  *ship = NULL,  *spinner = NULL;
vector<WireFrame> wireframes;
WireFrame wfspaceship, wfasteroid, wflaser;  // prototypes


int make_objects() {
	WireFrame obj;
	// spaceship
	obj.x = 100,  obj.y = 100;
	obj.hit_distance = 10;
	obj.points = { {{-10,-7}}, {{0,-15}}, {{10,-7}}, {{10,10}}, {{-10,10}}, {{-10,-7}} };
	obj.id = "spaceship";
	wfspaceship = obj;
	// asteroid
	obj.x = 200,  obj.y = 100;
	obj.hit_distance = 15;
	obj.points = { {{-5,-16}}, {{0,-18}}, {{12,-13}}, {{15,-4}}, {{9,12}}, {{0,16}}, {{-16,0}}, {{-5,-16}} };
	obj.id = "asteroid";
	obj.rotate = 30,  obj.speed = 1,  obj.drifttorque = 3;
	wfasteroid = obj;
	// laser
	obj.points = { {{0,-4}}, {{0,4}} };
	obj.id = "laser";
	obj.hit_distance = 3;
	// obj.x = wireframes[0].x,  obj.y = wireframes[0].y,  obj.rotate = wireframes[0].rotate;
	obj.speed = 5,  obj.drifttorque = 0;
	wflaser = obj;
	// add to scene
	wireframes.push_back(wfspaceship);
	wireframes.push_back(wfasteroid);
	return 0;
}


int make_laser() {
	printf("make laser\n");
	WireFrame obj = wflaser;
	obj.x = wireframes[0].x,  obj.y = wireframes[0].y,  obj.rotate = wireframes[0].rotate;
	wireframes.push_back(obj);
	return 0;
}


pair<WireFrame*, WireFrame*> idmatch(WireFrame* wf1, WireFrame* wf2, const string& id1, const string& id2) {
	if (wf1->id == id1 && wf2->id == id2) 
		return { wf1, wf2 };
	if (wf2->id == id1 && wf1->id == id2) 
		return { wf2, wf1 };
	return { NULL, NULL };
}


int make_asteroid(int type) {
	WireFrame obj = wfasteroid;
	obj.x = rand() % 320;
	obj.y = rand() % 240;
	obj.rotate = rand() % 360;
	if (type == 2) {
		obj.id += "2";
		obj.scale *= 0.5;
		obj.hit_distance *= 0.5;
	}
	wireframes.push_back(obj);
	return 0;
}


int main(int argc, char** argv) {
	printf("start\n");
	wireframes.reserve(200);  // prevent cascade reassigns
	gfx::init(640, 480, "asteroids");
	screen = SDL_GetVideoSurface();

	// make wireframes
	make_objects();

	int doloop=1;
	int action=0;
	while (doloop) {
		// move ship
		if (wireframes.size() && wireframes[0].id == "spaceship") {
			wireframes[0].speed  = (keys::u - keys::d) * 3;
			wireframes[0].torque = (keys::r - keys::l) * 5;
			if (keys::action && !action)  action = 10,  make_laser();
			if (!keys::action && action)  action = 0;
			// if (action)  action--;
		}

		// move characters
		for (auto& wf : wireframes) {
			wf.step();
			int offdir = 0;
			if (wf.x < -10)  wf.x = 320+20 + wf.x,  offdir = 1;
			if (wf.y < -10)  wf.y = 240+20 + wf.y,  offdir = 2;
			if (wf.x > 330)  wf.x = -10 + fmod(wf.x, 330),  offdir = 3;
			if (wf.y > 250)  wf.y = -10 + fmod(wf.y, 250),  offdir = 4;
			if (offdir && wf.id == "laser")  wf.flags = -1;
		}

		// collisions
		for (int i=wireframes.size()-1; i>=0; i--)
		for (int j=i-1; j>=0; j--) {
			// get info
			auto &w1 = wireframes[i],  &w2 = wireframes[j];
			if (w1.flags == -1 || w2.flags == -1)  continue;  // ignore dead
			double hit_dist = max(w1.hit_distance, w2.hit_distance);
			double dx = abs(w1.x - w2.x),  dy = abs(w1.y - w2.y);
			// printf("%s %s  %f %f %f\n", w1.id.c_str(), w1.id.c_str(), hit_dist, dx, dy);
			// do collision
			if (dx <= hit_dist && dy <= hit_dist) {
				// asteroid collisions - die
				auto pm = idmatch(&w1, &w2, "spaceship", "asteroid");
				if (pm.first != NULL)
					pm.first->flags = pm.second->flags = -1;
				pm = idmatch(&w1, &w2, "spaceship", "asteroid2");
				if (pm.first != NULL)
					pm.first->flags = pm.second->flags = -1;
				// laster shoots asteroid 1 - dissolve
				pm = idmatch(&w1, &w2, "laser", "asteroid");
				if (pm.first != NULL) {
					pm.first->flags = pm.second->flags = -1;
					for (int z=0; z<3; z++) {
						make_asteroid(2);
						wireframes.back().x = pm.second->x,  wireframes.back().y = pm.second->y;
					}
				}
				// laser shoots asteroid 2 - die
				pm = idmatch(&w1, &w2, "laser", "asteroid2");
				if (pm.first != NULL)
					pm.first->flags = pm.second->flags = -1;
			}
		}

		// erase dead
		for (int i=wireframes.size()-1; i>=0; i--)
			if (wireframes[i].flags==-1)
				wireframes.erase(wireframes.begin()+i);

		// redraw
		SDL_FillRect(screen, NULL, gfx::drawc(0,0,0));
		for (auto& wf : wireframes) 
			wf.draw();

		// flip and update
		gfx::scale2x(screen, screen);
		if (keys::update()==-1)  doloop=0;
		gfx::flip();		
	}

	SDL_Quit();
	return 0;
}