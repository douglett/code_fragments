#include <iostream>
#include <vector>
#include <cassert>
#include "game.h"

using namespace std;


// functions
static void keycb(int key, int state);
void move(int d);
// void corridor();
void repaint();
// vars
int running = 1;


int main() {
	cout << "start" << endl;
	// init
	xd::screen::init();
	xd::screen::keycb = keycb;
	draw::init();
	repaint();  // first paint
	// mainloop
	while (running) {
		if (xd::screen::paint())  break;
	}
	// cleanup
	xd::screen::quit();
}

static void keycb(int key, int state) {
	if (state != 1)  return;
	switch (key) {
	case 27:  running = 0;  break;
	case 0x40000052:  move(0);  break;  // up
	case 0x40000051:  move(2);  break;  // down
	case 0x4000004f:  move(1);  break;  // right
	case 0x40000050:  move(3);  break;  // left
	default:  printf("%x\n", key);
	}
}

void move(int d) {
	using namespace gmap;
	switch (d) {
	case 0:
		if (gmap::is_empty(getrow(1)[2])) {
			if      (dir == 0)  posy--, repaint();
			else if (dir == 1)  posx++, repaint();
			else if (dir == 2)  posy++, repaint();
			else if (dir == 3)  posx--, repaint();
		}
		break;
	case 2:
		if (gmap::is_empty(getrow(-1)[2])) {
			if      (dir == 0)  posy++, repaint();
			else if (dir == 1)  posx--, repaint();
			else if (dir == 2)  posy--, repaint();
			else if (dir == 3)  posx++, repaint();
		}
		break;
	case 1:  dir = (dir + 1) % 4;  repaint();  break;
	case 3:  dir = (dir + 3) % 4;  repaint();  break;
	default:  assert("wrong dir" == NULL);
	}
}

// void corridor() {
// 	auto* dat = xd::screen::backbuffer->getdata();
// 	uint32_t c = 0xffffffff;
// 	xd::draw::tracepoly(dat, 0, 0,  {{0, 0}, {13, 13}, {13, 86}, {0, 99}},  c);  // left row 1
// 	xd::draw::tracepoly(dat, 99, 0,  {{0, 0}, {-13, 13}, {-13, 86}, {0, 99}},  c);  // right row 1
// 	xd::draw::tracepoly(dat, 13, 13,  {{0, 0}, {15, 15}, {15, 58}, {0, 73}, {0, 0}},  c);  // left row 2
// 	xd::draw::tracepoly(dat, 99-13, 13,  {{0, 0}, {-15, 15}, {-15, 58}, {0, 73}, {0, 0}},  c);  // right row 2
// 	xd::draw::tracepoly(dat, 28, 28,  {{0, 0}, {0, 43}, {9, 34}, {9, 9}, {0, 0}},  c);  // left row 3
// 	xd::draw::tracepoly(dat, 99-28, 28,  {{0, 0}, {0, 43}, {-9, 34}, {-9, 9}, {0, 0}},  c);  // right row 3
// }

// void anim_test() {
// 	xd::screen::keycb = NULL;
// 	int sx = eye->x;
// 	int i = 0;
// 	while (running) {
// 		i = min(i + 8, 255);
// 		// eye->pos(sx + i, eye->y);
// 		auto* dat = eye->getdata();
// 		for (int j = 2; j < 2 + dat[0]*dat[1]; j++)
// 			dat[j] = (dat[j] & 0xffffff00) | i;
// 		if (i >= 255)  break;
// 		if (xd::screen::paint())  running = 0;
// 	}
// 	eye->x = sx;
// 	xd::screen::keycb = keycb;
// }

void repaint() {
	printf("x:%d y:%d  d:%d\n", gmap::posx, gmap::posy, gmap::dir);
	auto* dat = draw::eye->getdata();
	// xd::draw::clear(dat, 0x0000ffff);
	xd::draw::clear(dat);
	// draw foreground
	draw::far_row(dat);
	draw::back_row(dat);
	draw::mid_row(dat);
	draw::front_row(dat);
	// anim_test();
	draw::draw_compass();
	draw::draw_vmap();
}