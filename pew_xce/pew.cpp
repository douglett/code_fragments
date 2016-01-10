#include <iostream>
#include <string>
// #include <cmath.h>
#include "libsrc/xcengine.h"

using namespace std;


void player_collision();
void bullet_move_collide();
void enemy_move();
void stars_move();
void wave_advance();
void drawscreen();


// debug stuff
int total_frames_skipped = 0;
// general stuff
// bullet stuff
vector<SDL_Rect> bullets;

// enemy stuff
class Enemy {
public:
	// consts
	enum etype {
		ETYPE_NONE,
		ETYPE_BLOB_MONSTER,
		ETYPE_FLY,
		ETYPE_BAT
	};
	// members
	SDL_Rect rect = { 0, 0, 8, 8 };
	int type = 0;
	int speed = 1;
	int score = 100;
	int pattern = 0, step = 0;
	int subpattern[5] = { 0 };  // extra pattern handles
	int solid = 1, visible = 1;
	Enemy(int ttype) : type(ttype) { }
	char coltype() {
		switch (type) {
		case ETYPE_BLOB_MONSTER:
			game::qbcolor(0, 210, 0);
			return '&';
		case ETYPE_FLY:
			game::qbcolor(150, 210, 0);
			return 'v';
		case ETYPE_BAT:
			game::qbcolor(50, 50, 210);
			return 'M';
		}
		return '0';
	}
};
vector<Enemy> enemies;

// ship stuff
class Ship : public Enemy {
public:
	// consts
	static const int SHIP_TRIGGERTT_MAX = 8;
	// members
	int score = 0;
	int triggertt = 0;
	Ship() : Enemy(0) { }
};
Ship ship;

// stars
class Star {
public:
	SDL_Rect rect = { 0, 0, 8, 8 };
	int speed, shade = 200;
	Star(int tspeed, int x, int y) : speed(tspeed){
		rect.x = x;
		rect.y = y;
	}
};
vector<Star> stars;



// main loop
int main() {
	cout << "hello world" << endl;
	srand(time(NULL));
	rand();  // clear first result
	if (game::init())
		return 1;

	// setup the ship
	ship.rect.y = 220;
	ship.rect.x = game::width / 2;

	// debug stuff
	stringstream ss;
	int skip = 0;

	bool game_running = true;
	while (game_running) {
		// event setup
		if (game::clearevents())
			game_running = false;
		const Uint8* keys = SDL_GetKeyboardState(NULL);

		// state changes
		ship.triggertt++;
		player_collision();
		if (enemies.size() == 0)
			wave_advance();
		enemy_move();
		bullet_move_collide();
		stars_move();

		// handle quit key
		if (keys[ SDL_SCANCODE_ESCAPE ]) 
			game_running = false;
		// handle ship
		if (ship.solid) {
			// move left
			if (keys[ SDL_SCANCODE_A ] || keys[ SDL_SCANCODE_LEFT ]) {
				ship.rect.x -= ship.speed;
				if (ship.rect.x < 0)
					ship.rect.x = 0;
			}
			// move right
			if (keys[ SDL_SCANCODE_D ] || keys[ SDL_SCANCODE_RIGHT ]) {
				ship.rect.x += ship.speed;
				if (ship.rect.x > game::width - ship.rect.w - 1)
					ship.rect.x = game::width - ship.rect.w - 1;
			}
			// shoot bullet
			if (keys[ SDL_SCANCODE_SPACE ]) {
				if (ship.triggertt >= 0) {
					SDL_Rect b = ship.rect;
					b.y -= 8;
					bullets.push_back(b);
					ship.triggertt = -Ship::SHIP_TRIGGERTT_MAX;
				}
			}
		}

		// draw
		if (!skip) {
			drawscreen();
			skip = game::waitscreen();
			total_frames_skipped += skip;
		} else {
			skip = 0;
		}
	}

	game::quit();
}


void player_collision() {
	if (!ship.solid)
		return;
	for (auto &e : enemies)
		if (SDL_HasIntersection(&e.rect, &ship.rect)) {
			ship.solid = 0;
			ship.visible = 0;
		}
}


// check if bullets crash into stuff
void bullet_move_collide() {
	SDL_Rect *b, *e;
	bool collision = false;
	for (int j = 0; j < bullets.size(); j++) {
		collision = false;
		b = &bullets[j];
		b->y -= 3;

		for (int i = 0; i < enemies.size(); i++) {
			e = &enemies[i].rect;
			if (enemies[i].solid && SDL_HasIntersection(b, e)) {
				ship.score += enemies[i].score;
				enemies.erase(enemies.begin() + i);
				i--;
				collision = true;
				break;
			}
		}

		if (collision || b->y + b->h < 0) {
			bullets.erase(bullets.begin() + j);
			j--;
		}
	}
}


void stars_move() {
	for (auto &s : stars)
		s.rect.y += s.speed;

	if ( rand()%40 == 0 ) {
		int speed = rand()%10;
		if (speed < 7)
			speed = 1;
		else if (speed < 9)
			speed = 2;
		else
			speed = 3;
		Star s( speed, rand()%game::width, 0 );
		stars.push_back(s);
	}
}


void wave_advance() {
	static int wave = 2;
	static int step = 0;

	// advance wave after 2 seconds
	step++;
	if (step < 60 * 2)
		return;
	step = 0;
	wave++;

	switch (wave) {
	// first wave - green blob monsters
	case 1:
	{
		Enemy e( Enemy::ETYPE_BLOB_MONSTER );
		for (int y = 0; y < 3; y++)
			for (int x = 0; x < 20; x++) {
				e.rect.x = 80 + x*8;
				e.rect.y = 25 + y*8;
				e.step++;  // move the start position up by one
				enemies.push_back(e);  // copy to new array member
			}
		break;
	}
	// 10 random flies
	case 2:
	{
		Enemy e( Enemy::ETYPE_FLY );
		for (int i = 0; i < 10; i++) {
			e.rect.x = rand() % game::width;
			e.rect.y = rand() % int(game::height * 0.75);
			enemies.push_back(e);
		}
		break;
	}
	// some bats
	case 3:
	{
		Enemy e( Enemy::ETYPE_BAT );
		e.rect.y = 25;
		for (int x = 0; x < 20; x++) {
			e.rect.x = 80 + x*8;
			e.step += 2;
			enemies.push_back(e);
		}
		break;
	}
	// loop waves
	default:
		cout << "end waves. looping." << endl;
		wave = 0;
	}
}


// function defs
int blobmonster_move(Enemy& e);
int fly_move(Enemy& e);
int bat_move(Enemy& e);

// move enemies
void enemy_move() {
	int die;
	// for (auto &e : enemies) {
	for (int i = 0; i < enemies.size(); i++) {
		Enemy& e = enemies[i];
		e.step++;

		switch (e.type) {
		case Enemy::ETYPE_BLOB_MONSTER:
			die = blobmonster_move(e);
			break;
		case Enemy::ETYPE_FLY:
			die = fly_move(e);
			break;
		case Enemy::ETYPE_BAT:
			die = bat_move(e);
			break;
		}

		// wrap left/right
		e.rect.x = (e.rect.x + game::width) % game::width;
		// die of we flew off the top or bottom
		if (e.rect.y > game::height || e.rect.y + e.rect.h < 0)
			die = 1;

		if (die) {
			enemies.erase(enemies.begin() + i);
			i--;
		}
	}
}


int fly_move(Enemy& e) {
	switch (e.pattern) {
	// setup
	case 0:
		e.solid = 0;
		e.pattern++;
		break;
	// flash on and off
	case 1:
		e.visible = !e.visible;
		if (e.step >= 100) {
			e.visible = 1;
			e.solid = 1;
			e.pattern++;
			e.step = 0;
		}
		break;
	// pick random flight direction
	case 2:
		e.subpattern[0] = rand() % 5;
		e.pattern++;
		e.step = 0;
		// fallthrough
	// move in random direction for 15 frames
	case 3:
		// do movement
		{
			int r = e.subpattern[0];
			if (r == 0)
				e.rect.y -= e.speed;
			else if (r == 1)
				e.rect.x += e.speed;
			else if (r == 2 || r > 3)  // bias down
				e.rect.y += e.speed;
			else if (r == 3)
				e.rect.x -= e.speed;
		}
		// be ready to switch directions
		if (e.step > 15) {
			e.pattern = 2;
			e.step = 0;
		}
		break;
	} // end switch
	return 0;
}


int bat_move(Enemy& e) {
	switch (e.pattern) {
		// setup
	case 0:
		e.solid = 0;
		e.pattern++;
		break;
	// flash on and off
	case 1:
		e.visible = !e.visible;
		if (e.step >= 100) {
			e.visible = 1;
			e.solid = 1;
			e.pattern++;
			e.subpattern[0] = e.rect.y;
			e.step = 0;
		}
		break;
	// move down, right
	case 2:
		e.rect.x += 1;
		if (e.step % 3 == 0)
			e.subpattern[0] += 1;
		{
			static const double 
				PI = 3.142,
				OSCILATE = 30,
				AMPLITUDE = 20;
			double ratio = e.subpattern[0] / OSCILATE;
			e.rect.y = e.subpattern[0] + cos(ratio * PI * 2) * AMPLITUDE;
		}
		break;
	}
	return 0;
}


int blobmonster_move(Enemy& e) {
	switch (e.pattern) {
	// setup
	case 0:
		e.solid = 0;
		e.pattern++;
		break;
	// flash on and off
	case 1:
		// e.visible = (e.step / 2) % 2;
		e.visible = !e.visible;
		if (e.step >= 100) {
			e.visible = 1;
			e.solid = 1;
			e.pattern++;
			e.step = 0;
		}
		break;
	// move down, right
	case 2:
		e.rect.x += 1;
		e.rect.y += 1;
		if (e.step >= 20) {
			e.pattern++;
			e.step = 0;
		}
		break;
	// move down, left
	case 3:
		e.rect.x -= 1;
		e.rect.y += 1;
		if (e.step >= 20) {
			e.pattern++;
			e.step = 0;
		}
		break;
	// move up, left
	case 4:
		e.rect.x -= 1;
		e.rect.y -= 1;
		if (e.step >= 20) {
			e.pattern++;
			e.step = 0;
		}
		break;
	// move up, right
	case 5:
		e.rect.x += 1;
		e.rect.y -= 1;
		if (e.step >= 20) {
			e.subpattern[0]++;
			if (e.subpattern[0] < 3)
				e.pattern = 2;  // loop to start of movement
			else
				e.pattern++;
			e.step = 0;
		}
		break;
	// fly straight down fast!
	case 6:
		e.rect.y += 2;
		if (e.rect.y > game::height)
			return 1;
		break;
	}
	return 0;
}


// draw the screen
void drawscreen() {
	static stringstream ss;
	static string s;

	// CLS
	SDL_RenderClear(game::ren);

	// draw game
	game::qbcolor(255, 255, 255);
	for (auto &s : stars)
		game::qbprint(s.rect.x, s.rect.y, ".");
	game::qbcolor(210, 0, 0);
	if (ship.visible)
		game::qbprint(ship.rect.x, ship.rect.y, "A");
	game::qbcolor(255, 255, 0);
	for (auto &b : bullets)
		game::qbprint(b.x, b.y, ".");
	// draw all enemy types
	string c;
	for (auto &e : enemies)
		if (e.visible) {
			c = e.coltype();
			game::qbprint(e.rect.x, e.rect.y, c);
		}

	// score
	game::qbcolor(255, 255, 0);
	ss.str("");
	ss << ship.score;
	s = ss.str();
	game::qbprint(game::width - 9 - s.length()*8, 10, s);
	// enemy count
	ss.str("");
	ss << "e:" << enemies.size();
	s = ss.str();
	game::qbprint(game::width - 9 - s.length()*8, 20, s);

	// debug
	if (true) {
		game::qbcolor(255, 255, 255);

		// fps counter
		ss.str("");
		ss << "fps: " << game::FPS;
		game::qbprint(10, 10, ss.str());

		// total frame skip counter
		// ss.str("");
		// ss << "skip: " << total_frames_skipped;
		// game::qbprint(10, 20, ss.str());
	}
	
	SDL_RenderPresent(game::ren);
}
