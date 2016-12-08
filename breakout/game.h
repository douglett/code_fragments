#pragma once

#include <vector>
#include "xdlib/xdlib.h"

struct Obj {
	xd::screen::Sprite *sprite = NULL;
	float posx = 0, posy = 0;  // sub-pixel
	virtual void make() = 0;
	virtual void unmake();
	virtual void pos(int x, int y);
};

struct Paddle : Obj {
	void make();
};

struct Ball : Obj {
	float accelx = 1, accely = 1;
	void make();
};

struct Score : Obj {
	void make();
};

struct Brick : Obj {
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