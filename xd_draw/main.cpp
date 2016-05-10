#include <iostream>
#include <memory>
#include <algorithm>

#include "xd_screen/screen.h"
#include "libsrc/draw.h"

using namespace std;
using namespace xd;


int main() {
	cout << "hello world" << endl;

	xd::screen::init();

	// make hard sprite
	screen::Sprite* s = screen::makesprite(20, 20);
	s->pos(25, 25);
	draw::fillrect(s->getdata(), draw::rgb(150, 150, 0), 0, 0, 20, 20);

	// make a sprite
	uint32_t* img = draw::make_img(20, 20);
	draw::fillrect(img, draw::rgb(255, 255, 0), 0, 0, 20, 20);
	draw::fillrect(img, draw::rgb(200, 0, 0), 5, 5, 10, 10);
	int posx = 100, posy = 10;

	while (posy < 200) {
		uint32_t* scr = screen::backbuffer->getdata();  // get screen data
		draw::fillrect(scr, draw::rgb(0, 0, 0), 0, 0, screen::WIDTH, screen::HEIGHT);  // cls

		// draw some boxes
		draw::fillrect(scr, draw::rgb(255, 0, 0), -5, -5, 40, 40);
		draw::fillrect(scr, draw::rgb(0, 255, 0), 40, 40, 40, 40);
		draw::fillrect(scr, draw::rgb(0, 0, 255), 80, 80, 40, 40);

		draw::tracerect(scr, draw::rgb(255, 0, 0), 0, 0, 240, 160);
		draw::tracerect(scr, draw::rgb(255, 0, 0), -1, -1, 500, 500);

		draw::traceline(scr, draw::rgb(255, 0, 0), 100, 10, 100+80, 10+30);
		draw::traceline(scr, draw::rgb(255, 0, 0), 100, 10, 100+30, 10+80);
		draw::traceline(scr, draw::rgb(255, 0, 0), 100, 10, 100-30, 10-15);

		// draw sprite		
		draw::blit(img, scr, posx, posy);
		posx++;
		posy++;

		if (screen::paint())
			return 0;
	}
	
	SDL_Delay(1000);

	xd::screen::quit();
}