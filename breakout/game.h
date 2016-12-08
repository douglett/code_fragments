#pragma once

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

struct Brick : Obj {
	int col = 0;
	Brick(int col);
	void make();
	void tpos(int x, int y);
};