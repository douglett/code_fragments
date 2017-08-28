#include "graphics/graphics.h"
#include <iostream>

using namespace std;


int main(int argc, char** argv) {
	gfx::init(640, 480, "iso3d");
	SDL_Surface* scr = SDL_GetVideoSurface();

	while (true) {
		SDL_FillRect(scr, NULL, gfx::drawc(0,0,0));
		gfx::drawc(255,255,255);
		gfx::drawline(scr, 10, 10, 100, 100);

		gfx::flip();
	}

	SDL_Quit();
	return 0;
}