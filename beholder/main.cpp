#include <iostream>
#include <vector>
#include <cassert>
#include "xdlib/xdlib.h"

using namespace std;


// functions
static void keycb(int key, int state);
string getrow(int row);
void move(int d);
void corridor();
void repaint();
void back_row();
void mid_row();
void front_row();
void maketiles();
// consts
// const int ROWLEN[] = { 5, 5, 5 };
// vars
uint32_t* tiles[3][3] = { {NULL} };
int running = 1;
vector<string> map = {
	".....",
	".   .",
	". . .",
	".   .",
	".....",
};
int posx = 1, posy = 2, dir = 0;
namespace gmap {
	int is_empty(char c)   { return c == ' '; }
	int is_null(char c)    { return c == 'x'; }
	int is_nothing(char c) { return is_empty(c) || is_null(c); }
}


int main() {
	cout << "start" << endl;
	xd::screen::init();
	xd::screen::keycb = keycb;
	maketiles();
	repaint();

	while (running) {
		if (xd::screen::paint())  break;
	}

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

string getrow(int row) {
	string r;
	int  xx = posx,  yy = posy,  rsize = 2;
	// make row
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
	// format row
	// for (int i = 0; i < r.length(); i++) {
	// 	if (r[i] != '.')  continue;
	// 	else if (i > 0 && gmap::is_empty(r[i-1]))  r[i] = '\\';
	// 	else if (i < r.length()-1 && gmap::is_empty(r[i+1]))  r[i] = '/';
	// 	else  r[i] = '_';
	// }
	return r;
}

void move(int d) {
	switch (d) {
	case 0:
		if (gmap::is_empty(getrow(1)[2])) {
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
	// for (int i = 0; i < 5; i++)
	// 	if (gmap::is_nothing(row[i]))  continue;
	// 	// else if (i == 1)  xd::draw::tracepoly(dat, 28, 28, {{0, 0}, {9, 9}, {9, 34}, {0, 43}, {0, 0}, {-43, 0}, {-43, 43}, {0, 43}}, c);  // left row 2
	// 	// else if (i == 2)  xd::draw::tracerect(dat, 28, 28, 44, 44, c);  // mid row 2
	// 	else if (i == 1)  xd::draw::blit(tiles[2][0], dat, 0, 28);  // left row 2
	// 	else if (i == 2)  xd::draw::blit(tiles[2][1], dat, 28, 28);  // mid row 2
	// 	else if (i == 3)  xd::draw::tracepoly(dat, 99-28, 28, {{0, 0}, {-9, 9}, {-9, 34}, {0, 43}, {0, 0}, {43, 0}, {43, 43}, {0, 43}}, c);  // right row 2
	
	if (row[1] == '.')  xd::draw::blit(tiles[2][0], dat, 0, 28);  // left row 2
	if (row[2] == '.')  xd::draw::blit(tiles[2][1], dat, 28, 28);  // mid row 2
	if (row[3] == '.')  xd::draw::blit(tiles[2][2], dat, 99-28, 28);  // right row 2
}

void mid_row() {
	auto* dat = xd::screen::backbuffer->getdata();
	string row = getrow(1);
	printf("mid_row  : [%s]\n", row.c_str());
	if (row[1] == '.')  xd::draw::blit(tiles[1][0], dat, 0, 13);  // left row 1
	if (row[3] == '.')  xd::draw::blit(tiles[1][2], dat, 70, 13);  // right row 1
	if (row[2] == '.')  xd::draw::blit(tiles[1][1], dat, 13, 13);  // mid row 1
}

void front_row() {
	auto* dat = xd::screen::backbuffer->getdata();
	string row = getrow(0);
	printf("front_row: [%s]\n", row.c_str());
	if (row[1] == '.')  xd::draw::blit(tiles[0][0], dat, 0, 0);  // left row 0
	if (row[3] == '.')  xd::draw::blit(tiles[0][2], dat, 86, 0);  // right row 0
}

uint32_t* dupflip(const uint32_t* src) {
	auto* dat = xd::draw::make_img(src[0], src[1]);
	xd::draw::clear(dat);
	xd::draw::blit(src, dat, 0, 0);
	xd::draw::flip_h(dat);
	return dat;
}

void maketiles() {
	uint32_t c = 0xaa0000ff, b = 0x999999ff;
	uint32_t* unknown = xd::draw::make_img(10, 10);
	xd::draw::clear(unknown, 0xff00ffff);
	uint32_t* dat;
	// row-0 : left
	dat = tiles[0][0] = xd::draw::make_img(14, 100);
	xd::draw::clear(dat);
	xd::draw::tracepoly(dat, 0, 0,  {{0, 0}, {13, 13}, {13, 86}, {0, 99}},  c);
	xd::draw::fill(dat, 1, 2, b);
	// row-0 : mid
	tiles[0][1] = unknown;
	// row-0 : right
	tiles[0][2] = dupflip(tiles[0][0]);
	// row-1 : left
	dat = tiles[1][0] = xd::draw::make_img(30, 74);
	xd::draw::clear(dat);
	xd::draw::tracepoly(dat, 13, 0,  {{0, 0}, {15, 15}, {15, 58}, {0, 73}, {0, 0}, {-14, 0}, {-14, 73}, {0, 73}},  c);
	xd::draw::fill(dat, 1, 2, b);
	xd::draw::fill(dat, 14, 3, b);
	// row-1 : mid
	dat = tiles[1][1] = xd::draw::make_img(74, 74);
	xd::draw::clear(dat);
	xd::draw::tracerect(dat, 0, 0, 74, 74, c);
	xd::draw::fill(dat, 1, 2, b);
	// row-1 : right
	tiles[1][2] = dupflip(tiles[1][0]);
	// row-2 : left
	dat = tiles[2][0] = xd::draw::make_img(38, 44);
	xd::draw::clear(dat);
	xd::draw::tracepoly(dat, 28, 0, {{0, 0}, {9, 9}, {9, 34}, {0, 43}, {0, 0}, {-43, 0}, {-43, 43}, {0, 43}}, c);
	xd::draw::fill(dat, 1, 3, b);
	xd::draw::fill(dat, 29, 3, b);
	// row-2 : mid
	dat = tiles[2][1] = xd::draw::make_img(44, 44);
	xd::draw::clear(dat);
	xd::draw::tracerect(dat, 0, 0, 44, 44, c);
	xd::draw::fill(dat, 1, 2, b);
	// row-2 : right
	tiles[2][2] = unknown;
}