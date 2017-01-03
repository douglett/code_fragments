#include <iostream>
#include "xdlib/xdlib.h"

using namespace std;


void corridor();
void back_row();
void mid_row();
void front_row();


int main() {
	cout << "start" << endl;
	xd::screen::init();

	auto* dat = xd::screen::backbuffer->getdata();
	xd::draw::fillrect(dat, 0, 0, 100, 100, 0x0000ffff);
	// xd::draw::fillrect(dat, 0, 0, 100, 50, 0x0000ffff);
	// xd::draw::fillrect(dat, 0, 50, 100, 50, 0x664444ff);
	// corridor();
	// back_row();
	mid_row();
	// front_row();

	while (true) {
		if (xd::screen::paint())  break;
	}

	xd::screen::quit();
}


void corridor() {
	auto* dat = xd::screen::backbuffer->getdata();
	uint32_t c = 0xffffffff;
	xd::draw::tracepoly(dat, 0, 0,  {{0, 0}, {13, 13}, {13, 86}, {0, 99}},  c);  // left row 1
	xd::draw::tracepoly(dat, 99, 0,  {{0, 0}, {-13, 13}, {-13, 86}, {0, 99}},  c);  // right row 1
	xd::draw::tracepoly(dat, 13, 13,  {{0, 0}, {15, 15}, {15, 58}, {0, 73}, {0, 0}},  c);  // left row 2
	xd::draw::tracepoly(dat, 99-13, 13,  {{0, 0}, {-15, 15}, {-15, 58}, {0, 73}, {0, 0}},  c);  // right row 2
	xd::draw::tracepoly(dat, 28, 28,  {{0, 0}, {0, 43}, {9, 34}, {9, 9}, {0, 0}},  c);  // left row 3
	xd::draw::tracepoly(dat, 99-28, 28,  {{0, 0}, {0, 43}, {-9, 34}, {-9, 9}, {0, 0}},  c);  // right row 3
}

void back_row() {
	auto* dat = xd::screen::backbuffer->getdata();
	uint32_t c = 0xffffffff;
	xd::draw::tracerect(dat, 37, 37, 26, 26, c);
	xd::draw::tracerect(dat, 37-25, 37, 26, 26, c);
	xd::draw::tracerect(dat, 37+25, 37, 26, 26, c);
	xd::draw::tracerect(dat, 37-25*2, 37, 26, 26, c);
	xd::draw::tracerect(dat, 37+25*2, 37, 26, 26, c);
}

void mid_row() {
	auto* dat = xd::screen::backbuffer->getdata();
	uint32_t c = 0xffffffff;
	xd::draw::tracerect(dat, 28, 28, 44, 44, c);
	xd::draw::tracerect(dat, 28-43, 28, 44, 44, c);
	xd::draw::tracerect(dat, 28+43, 28, 44, 44, c);
}

void front_row() {
	auto* dat = xd::screen::backbuffer->getdata();
	uint32_t c = 0xff0000ff;
	xd::draw::tracerect(dat, 13, 13, 74, 74, c);
	xd::draw::tracerect(dat, 13-73, 13, 74, 74, c);
	xd::draw::tracerect(dat, 13+73, 13, 74, 74, c);
}