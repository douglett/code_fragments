#include <iostream>
#include <algorithm>
#include <SDL.h>
#include "libsrc/screen.h"


using namespace std;
using namespace xd;


int main() {
	cout << "hello world" << endl;
	screen::init();

	// box test
	// SDL_Rect r = { 5, 5, 10, 10 };
	// SDL_SetRenderDrawColor(screen::ren, 255, 0, 0, 255);
	// SDL_RenderFillRect(screen::ren, &r);

	// sprite test
	// screen::Sprite s(64, 64);
	auto& s = screen::makesprite(64, 64);
	s.pos(20, 20);
	uint32_t* d = s.getdata();
	fill_n(d+2, d[0]*d[1], screen::rgb(0, 255, 0));
	// s.flip();

	screen::paint();
	SDL_Delay(1000);

	s.pos(30, 30);
	screen::paint();
	SDL_Delay(1000);

	screen::quit();
}
