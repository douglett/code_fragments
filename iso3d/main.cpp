#include "graphics/graphics.h"
#include <iostream>
#include <vector>
#include <array>
#include <cmath>

using namespace std;


struct Model {
	double x=0, y=0, scale=1;
	double roll=0, pitch=0, yaw=0;
	vector<array<double, 6>> lines;
	void draw() {
		double t = M_PI/180 * roll;  // theta
		for (auto l : lines) {
			// translate
			double xt, yt;
			xt = l[0]*cos(t) - l[1]*sin(t);
			yt = l[0]*sin(t) + l[1]*cos(t);
			l[0]=xt, l[1]=yt;
			xt = l[3]*cos(t) - l[4]*sin(t);
			yt = l[3]*sin(t) + l[4]*cos(t);
			l[3]=xt, l[4]=yt;
			// draw
			gfx::drawline(SDL_GetVideoSurface(), 
				x + l[0] * scale,
				y + l[1] * scale,
				x + l[3] * scale,
				y + l[4] * scale );
		}
	}
};
vector<Model> models;


int main(int argc, char** argv) {
	gfx::init(640, 480, "iso3d");
	SDL_Surface* scr = SDL_GetVideoSurface();

	Model m;
	m.x=160, m.y=120;
	m.scale=30;
	m.roll=30;
	m.lines={
		{{-1,-1,0, 1,-1,0}},
		{{1,-1,0,  1,1,0}},
		{{1,1,0,  -1,1,0}},
		{{-1,1,0, -1,-1,0}},

		{{-1,-1,-1, 1,-1,-1}},
		{{1,-1,-1,  1,1,-1}},
		{{1,1,-1,  -1,1,-1}},
		{{-1,1,-1, -1,-1,-1}},
	};

	while (true) {
		SDL_FillRect(scr, NULL, gfx::drawc(0,0,0));
		gfx::drawc(255,255,255);
		// gfx::drawline(scr, 10, 10, 100, 100);

		m.draw();

		gfx::scale2x(scr, scr);
		gfx::flip();
	}

	SDL_Quit();
	return 0;
}