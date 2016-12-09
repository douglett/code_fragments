#pragma once

#include <vector>
#include "xdlib/xdlib.h"

namespace util {
	const  int  CSTR_MAX = 128;
	extern char cstr[CSTR_MAX];
}
#define strprintf(...) (snprintf (util::cstr, 100, __VA_ARGS__), util::cstr)

struct Obj {
	xd::screen::Sprite *sprite = NULL;
	float posx = 0, posy = 0;  // sub-pixel
	virtual void make() = 0;
	virtual void unmake();
	virtual void pos(float x, float y);
};

struct Paddle : Obj {
	float accel = 1.2;
	void make();
};

struct Ball : Obj {
	int    moving = 0;
	float  accelx = 1,  accely = 1;
	void make();
};

struct Score : Obj {
	int  score = 0,  lives = 3,  dirty = 0;
	void make();
	void add(int i);
	void die();
	void repaint();
};

struct Brick : Obj {
	static const int WIDTH = 12, HEIGHT = 12;
	int col = 0;
	Brick(int col);
	void make();
	void tpos(int x, int y);
};
const std::vector<std::pair<uint32_t, uint32_t>> BRICK_COL = {
	{ 0xFF0000FF, 0x990000FF },
	{ 0x0000FFFF, 0x000099FF },
	{ 0x00FF00FF, 0x009900FF },
	{ 0xFFFF00FF, 0x999900FF },
	{ 0xFF00FFFF, 0x990099FF },
};