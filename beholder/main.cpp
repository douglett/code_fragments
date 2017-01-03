#include <iostream>
#include "xdlib/xdlib.h"

using namespace std;


int main() {
	cout << "start" << endl;
	xd::screen::init();

	auto* dat = xd::screen::backbuffer->getdata();
	xd::draw::fillrect(dat, 0, 0, 100, 100, 0x0000ffff);
	// xd::draw::fillrect(dat, 0, 0, 100, 50, 0x0000ffff);
	// xd::draw::fillrect(dat, 0, 50, 100, 50, 0x664444ff);

	uint32_t c = 0xffffffff;
	// left row1
	// xd::draw::traceline(dat, 0, 0, 0, 99, c);
	xd::draw::traceline(dat, 0, 0, 13, 13, c);
	xd::draw::traceline(dat, 13, 13, 13, 86, c);
	xd::draw::traceline(dat, 13, 86, 0, 99, c);
	// right row1
	// xd::draw::traceline(dat, 99, 0, 99, 99, c);
	xd::draw::traceline(dat, 99, 0, 86, 13, c);
	xd::draw::traceline(dat, 86, 13, 86, 86, c);
	xd::draw::traceline(dat, 86, 86, 99, 99, c);
	// left row2
	xd::draw::traceline(dat, 13, 13, 13, 86, c);
	xd::draw::traceline(dat, 13, 13, 28, 28, c);
	xd::draw::traceline(dat, 28, 28, 28, 71, c);
	xd::draw::traceline(dat, 28, 71, 13, 86, c);
	// right row2
	xd::draw::traceline(dat, 99-13, 13, 99-13, 86, c);
	xd::draw::traceline(dat, 99-13, 13, 99-28, 28, c);
	xd::draw::traceline(dat, 99-28, 28, 99-28, 71, c);
	xd::draw::traceline(dat, 99-28, 71, 99-13, 86, c);
	// left row 3
	xd::draw::traceline(dat, 28, 28, 28, 71, c);
	xd::draw::traceline(dat, 28, 28, 37, 37, c);
	xd::draw::traceline(dat, 37, 37, 37, 62, c);
	xd::draw::traceline(dat, 37, 62, 28, 71, c);
	// right row 3
	xd::draw::traceline(dat, 99-28, 28, 99-28, 71, c);
	xd::draw::traceline(dat, 99-28, 28, 99-37, 37, c);
	xd::draw::traceline(dat, 99-37, 37, 99-37, 62, c);
	xd::draw::traceline(dat, 99-37, 62, 99-28, 71, c);

	while (true) {
		if (xd::screen::paint())  break;
	}

	xd::screen::quit();
}