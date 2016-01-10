#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <SDL.h>

#include "libsrc/xcengine.h"

using namespace std;


int move_player(int dx, int dy);
int move_box(int dx, int dy);
int next_level();
void draw();

SDL_Rect player = { 0, 0, 0, 0 };
int movecount = 0;
vector<string> map;
texture::Texture* qbfont;
string message = "push crates!";

int levelnum = -1;
typedef struct level_t {
	string name;
	vector<string> map;
} level;
vector<level> levellist = {
	{"easy peasy", {
		"########",
		"# @    #",
		"#      #",
		"#      #",
		"#  $ $ #",
		"#  . . #",
		"########"
	}},
	{"simples", {
		"  ###   ",
		"  #@#   ",
		"### ####",
		"#. $$ .#",
		"###  ###",
		"  #  #  ",
		"  ####  "
	}},
	{"hmmm!", {
		"##########",
		"#  .  .  #",
		"####$$## #",
		"#   $.   #",
		"#      @ #",
		"##########"
	}}
};


// mainloop
int main() {
	cout << "hello world" << endl;
	if (game::init())
		return 1;

	next_level();

	// main loop
	bool do_loop = true;
	while (do_loop) {
		// do key press stuff
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				do_loop = false;
				break;
			case SDL_WINDOWEVENT:
				break;
			case SDL_KEYDOWN:
				// handle keys
				switch(event.key.keysym.sym) {
				case SDLK_ESCAPE:
					do_loop = false;
					break;
				case SDLK_UP:
				case SDLK_w:
					move_player(0, -1);
					break;
				case SDLK_DOWN:
				case SDLK_s:
					move_player(0, 1);
					break;
				case SDLK_LEFT:
				case SDLK_a:
					move_player(-1, 0);
					break;
				case SDLK_RIGHT:
				case SDLK_d:
					move_player(1, 0);
					break;
				case SDLK_r:
					// reload this level
					levelnum--;
					next_level();
					break;
				}
				break;
			}  // end switch
		}  // end while

		// draw scene
		draw();

		// check for win state
		bool boxfound = false;
		for (auto &s : map)
			for (auto &c : s)
				if (c == '$')
					boxfound = true;
		if (!boxfound) {
			message = "you win!";
			draw();
			game::waitkey();
			if (next_level())
				break;
		}
	}

	game::quit();
}


// try to move player. returns 1 on bump.
int move_player(int dx, int dy) {
	char
		source = ' ',
		target = '@';
	
	if ( map[player.y + dy][player.x + dx] == '#' )  // player hits wall
		return 1;
	else if ( map[player.y + dy][player.x + dx] == '$' || map[player.y + dy][player.x + dx] == '*' )
		if (move_box(dx, dy))
			return 1;
	if ( map[player.y][player.x] == '+' )  // player walks from goal space
		source = '.';
	if ( map[player.y + dy][player.x + dx] == '.' )  // player walks to goal space
		target = '+';

	// move
	map[player.y][player.x] = source;
	player.x += dx;
	player.y += dy;
	map[player.y][player.x] = target;
	movecount++;
	return 0;
}


// try to move box. returns 1 on bump.
int move_box(int dx, int dy) {
	int 
		boxx = player.x + dx,
		boxy = player.y + dy;
	char
		source = ' ',
		target = '*';

	if ( map[boxy][boxx] == '*' )
		source = '.';

	// target can only be goal '.', or space ' '
	if ( map[boxy + dy][boxx + dx] == ' ' )
		target = '$';
	else if ( map[boxy + dy][boxx + dx] == '.' )
		target = '*';
	else
		return 1;

	map[boxy][boxx] = source;
	map[boxy + dy][boxx + dx] = target;
	return 0;
}


// draw scene
void draw() {
	SDL_SetRenderDrawColor(game::ren, 0, 0, 0, 255);
	SDL_RenderClear(game::ren);

	game::qbprint(10, 10, message);
	stringstream ss;
	ss << "moves: " << movecount;
	game::qbprint(10, 20, ss.str());
	// game::qbprint(qbfont->tex, 10, 20, "moves: ");

	// draw map
	for (int y = 0; y < map.size(); y++) {
		game::qbprint(10, 40 + y*10, map[y]);
	}

	SDL_RenderPresent(game::ren);
	SDL_Delay(17);
}


int next_level() {
	levelnum++;
	if (levelnum >= levellist.size())
		return 1;

	// load map
	map = levellist[levelnum].map;
	stringstream ss;
	ss << "level " << levelnum + 1 << ": " << levellist[levelnum].name;
	message = ss.str();

	// find player location
	for (int y = 0; y < map.size(); y++)
		for (int x = 0; x < map[y].length(); x++)
			if (map[y][x] == '@') {
				player.x = x;
				player.y = y;
			}

	// reset moves
	movecount = 0;

	return 0;
}
