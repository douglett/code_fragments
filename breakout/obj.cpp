#include <string>
#include <cassert>
#include "game.h"

using namespace std;


//*** Obj (base) ***
// void Brick::make();  // write
void Obj::unmake() {
	xd::screen::delsprite(sprite);
}
void Obj::pos(int x, int y) {
	posx = x;
	posy = y;
	sprite->pos(posx, posy);
}


// helper
static void outline(uint32_t* dat) {
	static const uint32_t fg = 0xeeee88ff;
	const int w = dat[0], h = dat[1];
	xd::draw::tracerect(dat, fg, 0, 0, w, h);
	dat[2 + 1*w + 1]       = 
	dat[2 + 2*w -2]        = 
	dat[2 + (h-2)*w + 1]   = 
	dat[2 + (h-1)*w - 2]   = fg;
	dat[2 + 0]             =
	dat[2 + w - 1]         =
	dat[2 + w*(h-1)]       =
	dat[2 + w*h - 1]       = 0x0;
}


//*** Paddle ***
void Paddle::make() {
	// screen info
	int screenw = 0, screenh = 0;
	xd::screen::getinfo(&screenw, &screenh, NULL);
	// make
	sprite = xd::screen::makesprite(8*5, 8, "paddle");
	sprite->zindex(100);
	sprite->pos((screenw - sprite->width())/2, screenh - (sprite->height() + 10));
	auto* dat = sprite->getdata();
	xd::draw::clear(dat, 0xff0000ff);
	outline(dat);
}


//*** Ball ***
void Ball::make() {
	// screen info
	int screenw = 0, screenh = 0;
	xd::screen::getinfo(&screenw, &screenh, NULL);
	// make
	sprite = xd::screen::makesprite(8, 8, "ball");
	sprite->zindex(100);
	auto* dat = sprite->getdata();
	xd::draw::clear(dat, 0xff0000ff);
	outline(dat);
}


//*** Score ***
void Score::make() {
	// screen info
	int screenw = 0, screenh = 0;
	xd::screen::getinfo(&screenw, &screenh, NULL);
	// make
	sprite = xd::screen::makesprite(10, 30, "score");
	sprite->pos(0, screenw - sprite->width());
	sprite->zindex(100);
	auto* dat = sprite->getdata();
	xd::text::prints(dat, "100", 0, 0);
}


//*** Brick ***
Brick::Brick(int col) {
	assert(col >= 0 && col < BRICK_COL.size());
	this->col = col;
	make();
}
void Brick::make() {
	uint32_t cl = BRICK_COL[col].first, cd = BRICK_COL[col].second;
	sprite = xd::screen::makesprite(8, 8);
	auto* dat = sprite->getdata();
	xd::draw::clear(dat, cl);
	xd::draw::tracerect(dat, cd, 0, 0, sprite->width(), sprite->height());
}
void Brick::tpos(int x, int y) {
	pos(x*8, y*8);
	sprite->pos(posx, posy);
}