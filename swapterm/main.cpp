#include "graphics/graphics.h"
#include <iostream>
#include <fstream>

using namespace std;


int main(int argc, char** argv) {
	gfx::init(640, 480, "swapterm");
	SDL_Surface* scr = SDL_GetVideoSurface();
	SDL_Surface* swapscr = gfx::mksprite(320, 240);

	fstream fs("/tmp/dswapd/vterm1.swap", fstream::in | fstream::binary);
	if (!fs.is_open())
		return fprintf(stderr, "error: could not open swap file\n"), 1;
	const int SSIZE = 320*240*sizeof(uint32_t);
	char scrbuf[SSIZE];

	while (true) {
		fs.seekp(128);
		// fs.read(scrbuf, SSIZE);
		fs.read((char*)swapscr->pixels, SSIZE);

		SDL_FillRect(scr, NULL, 0x00000000);
		SDL_Rect r={20, 20, 324, 244};
		SDL_FillRect(scr, &r, 0xffffff00);
		r.x+=2,  r.y+=2;
		SDL_BlitSurface(swapscr, NULL, scr, &r);
		gfx::flip();
	}

	SDL_Quit();
	return 0;
}