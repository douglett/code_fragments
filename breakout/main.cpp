#include <iostream>
#include "xdlib/xdlib.h"

using namespace std;


int running = 1;
int screenw = 0, screenh = 0;
xd::screen::Sprite  *paddle = NULL;
int joy = 0;


struct Ball {
	xd::screen::Sprite *sprite = NULL;
	int accelx = 1, accely = 1;
	void make() {
		int screenw = 0, screenh = 0;
		xd::screen::getinfo(&screenw, &screenh, NULL);
		sprite = xd::screen::makesprite(8, 8, "ball");
		sprite->zindex(100);
		xd::draw::clear(sprite->getdata(), 0xff0000ff);
	}
};


struct Brick {
	xd::screen::Sprite *sprite = NULL;
	int posx = 0, posy = 0;
	Brick(int col) {
		make(col);
	}
	Brick(int col, int x, int y) {
		make(col);
		pos(x, y);
	}
	void make(int col) {
		sprite = xd::screen::makesprite(8, 8);
		xd::draw::clear(sprite->getdata(), 0xff0000ff);
		xd::draw::tracerect(sprite->getdata(), 0x990000ff, 0, 0, sprite->width(), sprite->height());
	}
	void pos(int x, int y) {
		posx = x;
		posy = y;
		sprite->pos(posx*8, posy*8);
	}
};


void keycb(int key, int state) {
	switch (key) {
	case 27:  running = 0;  break;
	case 0x40000050:  joy = -state;  break;  // left
	case 0x4000004f:  joy = state;  break;  // right
	case 0x40000052:  break;  // up
	case 0x40000051:  break;  // down
	// default:  printf("%x \n", key);
	}
}


int main() {
	cout << "hello world" << endl;
	cout << (-10 / abs(-10)) << endl;
	xd::screen::init();
	xd::screen::getinfo(&screenw, &screenh, NULL);
	printf("%d %d\n", screenw, screenh);
	xd::screen::keycb = keycb;
	// paddle
	paddle = xd::screen::makesprite(8*5, 8, "paddle");
	xd::draw::clear(paddle->getdata(), 0xff0000ff);
	paddle->zindex(100);
	paddle->pos((screenw - paddle->width())/2, screenh - (paddle->height() + 10));
	// ball
	Ball ball;
	ball.make();
	ball.sprite->pos(100, 100);
	// bricks
	vector<Brick> bricks;
	// for (int i = 0; i < 10; i++)
	// 	bricks.push_back( Brick(0, i+2, 2) );

	while (running) {
		// move paddle
		if       (joy == -1 && paddle->x > 0)  paddle->pos(paddle->x - 1, paddle->y);
		else if  (joy == 1 && paddle->x + paddle->width() < screenw)  paddle->pos(paddle->x + 1, paddle->y);
		// move ball
		ball.sprite->x += ball.accelx;
		ball.sprite->y += ball.accely;
		if       (ball.sprite->x < 0)  ball.accelx = 1;
		else if  (ball.sprite->x + ball.sprite->width() >= screenw)  ball.accelx = -1;
		else if  (ball.sprite->y < 0)  ball.accely = 1;
		else if  (ball.sprite->y + ball.sprite->height() >= screenh)  ball.accely = -1;
		// paddle intersections
		if (paddle->intersects(ball.sprite)) {
			ball.accely = ( ball.sprite->y + ball.sprite->height()/2 < paddle->y + paddle->height()/2 ? -1 : 1 );
			ball.accelx = ( ball.sprite->x + ball.sprite->width()/2  < paddle->x + paddle->width()/2  ? -1 : 1 );
		}
		// brick intersections
		// for (auto& b : bricks)
		// 	if (b.sprite->intersects(ball.sprite)) {
		// 		int ydiff = ( ball.sprite->y + ball.sprite->height()/2 - b.sprite->y + b.sprite->height()/2 );
		// 		int xdiff = ( ball.sprite->x + ball.sprite->width()/2  - b.sprite->x + b.sprite->width()/2  );
		// 		if (abs(xdiff) < abs(ydiff)) {
		// 			if    (xdiff < 0)  ball.accelx = -1;
		// 			else  ball.accelx = 1;
		// 		} else {
		// 			if    (ydiff < 0)  ball.accely = -1;
		// 			else  ball.accely = 1;
		// 		}
		// 		// printf("x %d  y %d\n", xdiff, ydiff);
		// 		break;
		// 	}
		// repaint
		if (xd::screen::paint())  break;
	}

	xd::screen::quit();
}