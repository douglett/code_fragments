#include <string>
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


//*** Paddle ***
void Paddle::make() {
	// screen info
	int screenw = 0, screenh = 0;
	xd::screen::getinfo(&screenw, &screenh, NULL);
	// make
	sprite = xd::screen::makesprite(8*5, 8, "paddle");
	xd::draw::clear(sprite->getdata(), 0xff0000ff);
	sprite->zindex(100);
	sprite->pos((screenw - sprite->width())/2, screenh - (sprite->height() + 10));
}


//*** Ball ***
void Ball::make() {
	// screen info
	int screenw = 0, screenh = 0;
	xd::screen::getinfo(&screenw, &screenh, NULL);
	// make
	sprite = xd::screen::makesprite(8, 8, "ball");
	sprite->zindex(100);
	xd::draw::clear(sprite->getdata(), 0xff0000ff);
}


//*** Brick ***
Brick::Brick(int col) {
	this->col = col;
	make();
}
void Brick::make() {
	sprite = xd::screen::makesprite(8, 8);
	xd::draw::clear(sprite->getdata(), 0xff0000ff);
	xd::draw::tracerect(sprite->getdata(), 0x990000ff, 0, 0, sprite->width(), sprite->height());
}
void Brick::tpos(int x, int y) {
	pos(x*8, y*8);
	sprite->pos(posx, posy);
}