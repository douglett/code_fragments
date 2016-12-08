#include <iostream>
#include <vector>
#include "game.h"

using namespace std;


// vars
int screenw = 0, screenh = 0;
int running = 1;
int joy = 0;
// game vars
Paddle paddle;
Ball ball;
Score score;
vector<Brick> bricks;


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
	// init xd
	xd::screen::init();
	xd::screen::getinfo(&screenw, &screenh, NULL);
	printf("%d %d\n", screenw, screenh);
	xd::screen::keycb = keycb;
	// paddle
	paddle.make();
	paddle.pos((screenw - paddle.sprite->width())/2, screenh - (paddle.sprite->height() + 10));
	// ball
	ball.make();
	ball.pos(100, 100);
	// bricks
	srand(1000);
	for (int i = 0; i < 10; i++) {
		Brick b( rand() % BRICK_COL.size() );
		b.tpos(i+5, 5);
		bricks.push_back(b);
	}

	while (running) {
		// move ball
		ball.pos(ball.posx + ball.accelx, ball.posy + ball.accely);
		// screen intersect
		if       (ball.posx < 0)  ball.accelx = 1;
		else if  (ball.posx + ball.sprite->width() >= screenw)  ball.accelx = -1;
		else if  (ball.posy < 0)  ball.accely = 1;
		else if  (ball.posy + ball.sprite->height() >= screenh)  ball.accely = -1;
		// paddle intersections
		if (paddle.sprite->intersects(ball.sprite)) {
			ball.accely = ( ball.sprite->center('y') < paddle.sprite->center('y') ? -1 : 1 );
			ball.accelx = ( ball.sprite->center('x') < paddle.sprite->center('x') ? -1 : 1 );
		}
		// brick intersections
		int  bax = ball.sprite->center('x'),  bay = ball.sprite->center('y');
		for (int i = 0; i < bricks.size(); i++)
			if (bricks[i].sprite->intersects(ball.sprite)) {
				int  bx = bricks[i].sprite->center('x'),  by = bricks[i].sprite->center('y');
				if (abs(bax - bx) > abs(bay - by))  ball.accelx = ( bax - bx < 0 ? -1 : +1 );
				else  ball.accely = ( bay - by < 0 ? -1 : +1 );
				bricks[i].unmake();
				bricks.erase(bricks.begin()+i);  // kill brick
				break;  // only process 1 intersect per loop
			}
		// move paddle
		if       (joy == -1 && paddle.posx > 0)  paddle.pos(paddle.posx - 1, paddle.posy);
		else if  (joy ==  1 && paddle.sprite->max('x') < screenw)  paddle.pos(paddle.posx + 1, paddle.posy);
		// repaint
		if (xd::screen::paint())  break;
	}

	xd::screen::quit();
}