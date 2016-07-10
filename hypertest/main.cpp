#include <iostream>
#include "xdlib/xdlib.h"

using namespace std;


int main(int argc, char** argv) {
	printf("hello world\n");

	xd::screen::init();

	string s = "hello world!";
	auto spr = xd::screen::makesprite(8*s.length()+2, 8+2);
	spr->pos(10, 2);
	xd::draw::fillrect(spr->getdata(), xd::draw::rgba(0, 255, 0, 100), 0, 0, 8*s.length()+2, 8+2);
	xd::text::prints(spr->getdata(), s, 1, 1, xd::draw::rgb(255, 0, 0));

	auto dscreen = xd::screen::makesprite(128+8, 96+8);
	dscreen->pos(10, 14);
	xd::draw::fillrect(dscreen->getdata(), xd::draw::rgb(255, 255, 0), 0, 0, 128+8, 96+8);
	for (int y = 0; y < 12; y++)
		for (int x = 0; x < 16; x++)
			xd::text::prints(dscreen->getdata(), "x", x*8+4, y*8+4);

	// spr->z = 1;
	// xd::screen::resort();
	spr->zindex(1);

	while (true) {
		if (xd::screen::paint())
			break;
	}

	xd::screen::quit();

	return 0;
}