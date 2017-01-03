#include <iostream>
#include <vector>
#include <cassert>
#include "xdlib/xdlib.h"

using namespace std;


// fn
static void keycb(int key, int state);
string getrow(int row);
void move(int d);
void corridor();
void repaint();
void back_row();
void mid_row();
void front_row();
// vars
vector<string> map = {
	".....",
	".   .",
	".   .",
	".   .",
	".....",
};
int posx = 1, posy = 2, dir = 0;


int main() {
	cout << "start" << endl;
	xd::screen::init();
	xd::screen::keycb = keycb;

	// auto* dat = xd::screen::backbuffer->getdata();
	// xd::draw::fillrect(dat, 0, 0, 100, 100, 0x0000ffff);
	// // xd::draw::fillrect(dat, 0, 0, 100, 50, 0x0000ffff);
	// // xd::draw::fillrect(dat, 0, 50, 100, 50, 0x664444ff);
	// // for (int i = 0; i < 4; i++) {
	// // 	dir = i;
	// // 	for (int i = 0; i < 3; i++)
	// // 		printf("[%s]\n", getrow(i).c_str());
	// // }
	// // corridor();
	// back_row();
	// mid_row();
	// front_row();
	repaint();

	while (true) {
		if (xd::screen::paint())  break;
	}

	xd::screen::quit();
}

static void keycb(int key, int state) {
	if (state != 1)  return;
	switch (key) {
	case 0x40000052:  move(0);  break;  // up
	case 0x40000051:  move(2);  break;  // down
	case 0x4000004f:  move(1);  break;  // right
	case 0x40000050:  move(3);  break;  // left
	default:  printf("%x\n", key);
	}
}

string getrow(int row) {
	string r;
	int xx = posx, yy = posy, rsize = (row == 2 ? 2 : 1);
	switch (dir) {
	case 0:  // north
		yy -= row;
		for (int x = -rsize; x <= rsize; x++)
			if (yy < 0 || yy >= map.size() || xx+x < 0 || xx+x >= map[yy].size())  r += 'x';
			else  r += map[yy][xx+x];
		break;
	case 1:  // east
		xx += row;
		for (int y = -rsize; y <= rsize; y++)
			if (yy+y < 0 || yy+y >= map.size() || xx < 0 || xx >= map[yy+y].size())  r += 'x';
			else  r += map[yy+y][xx];
		break;
	case 2:  // south
		yy += row;
		for (int x = rsize; x >= -rsize; x--)
			if (yy < 0 || yy >= map.size() || xx+x < 0 || xx+x >= map[yy].size())  r += 'x';
			else  r += map[yy][xx+x];
		break;
	case 3:  // west
		xx -= row;
		for (int y = rsize; y >= -rsize; y--)
			if (yy+y < 0 || yy+y >= map.size() || xx < 0 || xx >= map[yy+y].size())  r += 'x';
			else  r += map[yy+y][xx];
		break;
	}
	return r;
}

void move(int d) {
	switch (d) {
	case 0:
		if (getrow(1)[1] == ' ') {
			if      (dir == 0)  posy--, repaint();
			else if (dir == 1)  posx++, repaint();
			else if (dir == 2)  posy++, repaint();
			else if (dir == 3)  posx--, repaint();
		}
		break;
	case 1:  dir = (dir + 1) % 4;  repaint();  break;
	case 2:  break;
	case 3:  dir = (dir + 3) % 4;  repaint();  break;
	default:  assert("wrong dir" == NULL);
	}
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

void repaint() {
	auto* dat = xd::screen::backbuffer->getdata();
	xd::draw::clear(dat);
	xd::draw::fillrect(dat, 0, 0, 100, 100, 0x0000ffff);
	printf("x:%d y:%d  d:%d\n", posx, posy, dir);
	back_row(), mid_row(), front_row();
}

void back_row() {
	auto* dat = xd::screen::backbuffer->getdata();
	uint32_t c = 0x770000ff;
	// xd::draw::tracerect(dat, 37, 37, 26, 26, c);
	// xd::draw::tracerect(dat, 37-25, 37, 26, 26, c);
	// xd::draw::tracerect(dat, 37+25, 37, 26, 26, c);
	// xd::draw::tracerect(dat, 37-25*2, 37, 26, 26, c);
	// xd::draw::tracerect(dat, 37+25*2, 37, 26, 26, c);
	string row = getrow(2);
	printf("back_row : [%s]\n", row.c_str());
	for (int i = 0; i < 5; i++)
		if (row[i] == '.')
			xd::draw::tracerect(dat, 37+(25 * (i-2)), 37, 26, 26, c);
}

void mid_row() {
	auto* dat = xd::screen::backbuffer->getdata();
	uint32_t c = 0xaa0000ff;
	// xd::draw::tracerect(dat, 28, 28, 44, 44, c);
	// xd::draw::tracerect(dat, 28-43, 28, 44, 44, c);
	// xd::draw::tracerect(dat, 28+43, 28, 44, 44, c);
	string row = getrow(1);
	printf("mid_row  : [%s]\n", row.c_str());
	for (int i = 0; i < 3; i++)
		if (row[i] == '.')
			xd::draw::tracerect(dat, 28+(43 * (i-1)), 28, 44, 44, c);
}

void front_row() {
	auto* dat = xd::screen::backbuffer->getdata();
	uint32_t c = 0xff0000ff;
	// xd::draw::tracerect(dat, 13, 13, 74, 74, c);
	// xd::draw::tracerect(dat, 13-73, 13, 74, 74, c);
	// xd::draw::tracerect(dat, 13+73, 13, 74, 74, c);
	string row = getrow(0);
	printf("front_row: [%s]\n", row.c_str());
	for (int i = 0; i < 3; i++)
		if (row[i] == '.')
			xd::draw::tracerect(dat, 13+(73 * (i-1)), 13, 74, 74, c);
}