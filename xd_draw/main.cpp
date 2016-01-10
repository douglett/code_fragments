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

	// make a sprite
	uint32_t* img = draw::make_img(20, 20);
	draw::fillrect(img, draw::rgb(255, 255, 0), 0, 0, 20, 20);
	draw::fillrect(img, draw::rgb(200, 0, 0), 5, 5, 10, 10);
	int posx = 100, posy = 10;

	while (posy < 200) {
		// cls
		uint32_t* buf = screen::backbuffer->getdata();
		draw::fillrect(buf, draw::rgb(0, 0, 0), 0, 0, screen::WIDTH, screen::HEIGHT);

		// draw some boxes
		draw::fillrect(buf, draw::rgb(255, 0, 0), -5, -5, 40, 40);
		draw::fillrect(buf, draw::rgb(0, 255, 0), 40, 40, 40, 40);
		draw::fillrect(buf, draw::rgb(0, 0, 255), 80, 80, 40, 40);

		// draw sprite		
		draw::blit(img, buf, posx, posy);
		posx++;
		posy++;

		screen::paint();
	}
	
	SDL_Delay(1000);

	xd::screen::quit();
}