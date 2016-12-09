#include <iostream>
#include <vector>
#include "game.h"

using namespace std;


// pre-def
int  scoreloop();
int  mainloop();
void keycb(int key, int state);
void switchlevel(int lvl);
// vars
int screenw = 0, screenh = 0;
int running = 1, gamestate = 0;
int joy = 0;
// game vars
Paddle         paddle;
Ball           ball;
Score          score;
HighScore      highscore;
vector<Brick>  bricks;



int main() {
	cout << "hello world" << endl;
	// init xd
	xd::screen::init();
	xd::screen::getinfo(&screenw, &screenh, NULL);
	printf("%d %d\n", screenw, screenh);
	xd::screen::keycb = keycb;
	// do stuff
	while (running) {
		if (scoreloop())  break;
		if (mainloop())   break;
	}
	// exit
	xd::screen::quit();
}


int scoreloop() {
	// setup
	gamestate = 1;
	highscore.make();
	// loop
	while (running) {
		if (gamestate != 1)  break;
		if (xd::screen::paint())  return 1;
	}
	// setdown...
	xd::draw::clear(xd::screen::backbuffer->getdata(), 0x000000ff);
	return 0;
}


int mainloop() {
	// setup
	gamestate = 2;
	paddle.make();
	paddle.pos((screenw - paddle.sprite->width())/2, screenh - (paddle.sprite->height() + 10));
	ball.make(), ball.reset();
	score.make(), score.reset();
	switchlevel(1);
	// mainloop
	while (running) {
		// move ball
		if (ball.moving)  ball.pos(ball.posx + ball.accelx, ball.posy + ball.accely);
		// screen intersect
		if       (ball.posx < 0)  ball.accelx = 1;
		else if  (ball.posx + ball.sprite->width()  >= screenw)  ball.accelx = -1;
		else if  (ball.posy < 0)  ball.accely = 1;
		else if  (ball.posy + ball.sprite->height() >= screenh) {
			ball.reset();
			score.die();
			if (score.lives <= 0)  break;
		}
		// paddle intersections
		if (paddle.sprite->intersects(ball.sprite)) {
			ball.accely = ( ball.sprite->center('y') < paddle.sprite->center('y') ? -1 : 1 );
			ball.accelx = ( ball.sprite->center('x') < paddle.sprite->center('x') ? -1 : 1 );
		}
		// brick intersections
		int  bax = ball.sprite->center('x'),  bay = ball.sprite->center('y');
		for (int i = 0; i < bricks.size(); i++)
			if (bricks[i].sprite->intersects(ball.sprite)) {
				// check for x or y dominant collision, for bouncing
				int  bx = bricks[i].sprite->center('x'),  by = bricks[i].sprite->center('y');
				if (abs(bax - bx) > abs(bay - by))  ball.accelx = ( bax - bx < 0 ? -1 : +1 );
				else  ball.accely = ( bay - by < 0 ? -1 : +1 );
				// kill brick
				score.add((bricks[i].col + 1) * 100);
				bricks[i].unmake();
				bricks.erase(bricks.begin()+i);
				break;  // only process 1 intersect per loop
			}
		// move paddle
		if       (joy == -1 && paddle.posx > 0)  paddle.pos(paddle.posx - paddle.accel, paddle.posy);
		else if  (joy ==  1 && paddle.sprite->max('x') < screenw)  paddle.pos(paddle.posx + paddle.accel, paddle.posy);
		// repaint
		if (score.dirty)  score.repaint();
		if (xd::screen::paint())  return 1;
	} // end while
	// unset
	paddle.unmake();
	ball.unmake();
	score.unmake();
	switchlevel(0);
	// add new score
	highscore.add(score.score);
	return 0;
}


void keycb(int key, int state) {
	switch (key) {
	// control keys
	case 27:   running = 0;  break;
	case ' ':
		if (state == 1 && gamestate == 1)  gamestate = 2;
		if (state == 1 && gamestate == 2)  ball.moving = 1;
		break;
	// directions
	case 'a':
	case 0x40000050:  joy = -state;  break;  // left
	case 'd':
	case 0x4000004f:  joy =  state;  break;  // right
	case 0x40000052:  break;  // up
	case 0x40000051:  break;  // down
	// default:  printf("%x \n", key);
	}
}


void switchlevel(int lvl) {
	// erase bricks
	for (auto& b : bricks)
		b.unmake();
	bricks.erase(bricks.begin(), bricks.end());
	// make new bricks 
	switch (lvl) {
	case 1:
		int bwl = 16, bwh = 5;
		for (int j = 0; j < bwh; j++)
			for (int i = 0; i < bwl; i++) {
				int col = 0;
				if       (i > 1 && i < bwl-2 && j > 1 && j < bwh-2)  col = 2;
				else if  (i > 0 && i < bwl-1 && j > 0 && j < bwh-1)  col = 1;
				Brick b(col);
				b.tpos(i+2, j+1);
				bricks.push_back(b);
			}
		break;
	} // end switch
}