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
		xd::draw::clear(sprite->getdata(), 0xff0000ff);
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
	xd::screen::init();
	xd::screen::getinfo(&screenw, &screenh, NULL);
	printf("%d %d\n", screenw, screenh);
	xd::screen::keycb = keycb;
	// paddle
	paddle = xd::screen::makesprite(8*5, 8, "paddle");
	xd::draw::clear(paddle->getdata(), 0xff0000ff);
	paddle->pos((screenw - paddle->width())/2, screenh - (paddle->height() + 10));
	// ball
	Ball ball;
	ball.make();

	while (running) {
		// move paddle
		if       (joy == -1 && paddle->x > 0)  paddle->pos(paddle->x - 1, paddle->y);
		else if  (joy == 1 && paddle->x + paddle->width() < screenw)  paddle->pos(paddle->x + 1, paddle->y);
		// move ball
		ball.sprite->x += ball.accelx;
		ball.sprite->y += ball.accely;
		if (ball.sprite->x < 0 || ball.sprite->x + ball.sprite->width() >= screenw)   ball.accelx *= -1;
		if (ball.sprite->y < 0 || ball.sprite->y + ball.sprite->height() >= screenh)  ball.accely *= -1;
		if (paddle->intersects(ball.sprite)) { ball.accelx *= -1;  ball.accely *= -1; }
		// repaint
		if (xd::screen::paint())  break;
	}

	xd::screen::quit();
}