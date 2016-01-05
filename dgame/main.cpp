#include <iostream>
#include <algorithm>
#include <SDL.h>
#include "screen.h"


using namespace std;


using namespace xd;
using namespace xd::screen;


int main() {
	cout << "hello world" << endl;
	screen::init();

	// box test
	SDL_Rect r = { 5, 5, 10, 10 };
	SDL_SetRenderDrawColor(screen::ren, 255, 0, 0, 255);
	SDL_RenderFillRect(screen::ren, &r);

	// sprite test
	Sprite s(64, 64);
	s.dst.x = 20;
	s.dst.y = 20;
	uint32_t* d = s.getdata();
	fill_n(d+2, d[0]*d[1], rgb(0, 255, 0));
	s.flip();

	screen::flip();
	SDL_Delay(2000);

	screen::quit();
}
