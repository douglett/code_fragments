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
	xd::draw::tracepoly(dat, 0, 0,  {{0, 0}, {13, 13}, {13, 86}, {0, 99}},  c);
	// right row1
	xd::draw::tracepoly(dat, 99, 0,  {{0, 0}, {-13, 13}, {-13, 86}, {0, 99}},  c);
	// left row2
	xd::draw::tracepoly(dat, 13, 13,  {{0, 0}, {15, 15}, {15, 58}, {0, 73}, {0, 0}},  c);
	// right row2
	xd::draw::tracepoly(dat, 99-13, 13,  {{0, 0}, {-15, 15}, {-15, 58}, {0, 73}, {0, 0}},  c);
	// left row 3
	xd::draw::tracepoly(dat, 28, 28,  {{0, 0}, {0, 43}, {9, 34}, {9, 9}, {0, 0}},  c);
	// right row 3
	xd::draw::tracepoly(dat, 99-28, 28,  {{0, 0}, {0, 43}, {-9, 34}, {-9, 9}, {0, 0}},  c);

	while (true) {
		if (xd::screen::paint())  break;
	}

	xd::screen::quit();
}