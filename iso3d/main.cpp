#include "graphics/graphics.h"
#include <iostream>
#include <vector>
#include <array>

using namespace std;


struct Model {
	double x=0, y=0, scale=1;
	vector<array<int, 6>> lines;
	void draw() {
		for (const auto& l : lines) {
			gfx::drawline(SDL_GetVideoSurface(), 
				x+l[0]*scale, y+l[1]*scale,
				x+l[3]*scale, y+l[4]*scale );
		}
	}
};
vector<Model> models;


int main(int argc, char** argv) {
	gfx::init(640, 480, "iso3d");
	SDL_Surface* scr = SDL_GetVideoSurface();

	Model m;
	m.x=160, m.y=120, m.scale=30;
	m.lines={
		{{-1,-1,0,1,-1,0}},
		{{1,-1,0,1,1,0}},
		{{1,1,0,-1,1,0}},
		{{-1,1,0,-1,-1,0}}
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