#include <iostream>
#include "libsrc/xcengine.h"

using namespace std;


enum Action {
	ACT_NONE,
	ACT_WEST,
	ACT_EAST,
	ACT_NORTH,
	ACT_SOUTH,
	ACT_ACTION
};

void draw();
void createmap();
int handleevents();
int playeraction(Action action);
int collide(int x, int y);
void centercam();

class mob {
public:
	int x = 0;
	int y = 0;
	int type = 0;
};

SDL_Texture* sprites = NULL;
const SDL_Rect 
		parchment = { 0, 0, 100, 28 },
		cardback = { 0, 29, 16, 18 },
		spade = { 17, 30, 14, 16 },
		heart = { 31, 30, 14, 16 },
		club = { 47, 30, 15, 16 },
		diamond = { 62, 30, 14, 16 },
		man = { 0, 48, 12, 12 },
		scorpion = { 24, 48, 12, 12 },
		cake = { 48, 48, 12, 12 };

vector<vector<int> > map;
int animtt = 0, animstate = 0;
mob playermob;
mob camera;
vector<mob> mobs;



int main() {
	cout << "hello world" << endl;
	if (game::init())
		return 1;

	createmap();
	playermob.x = 4, playermob.y = 3;
	centercam();

	// sprite images
	sprites = texture::get("images")->tex;

	while (true) {
		animtt++;
		if (animtt % 30 == 0) {
			animtt = 0;
			animstate = !animstate;
		}

		draw();
		SDL_RenderPresent(game::ren);
		game::waitscreen();
		
		// if (game::clearevents())
		// 	break;
		if (handleevents())
			break;
	}

	game::quit();
}



void createmap() {
	// clear old
	map.erase(map.begin(), map.end());
	mobs.erase(mobs.begin(), mobs.end());

	srand(time(NULL));

	// create map
	for (int y = 0; y < 20; y++) {
		map.push_back(vector<int>());
		for (int x = 0; x < 20; x++) {
			if (x == 0 || y == 0 || x == 19 || y == 19)
				map[y].push_back( -2 );
			else
				map[y].push_back( rand()%2 );
		}
	}

	// make mobs
	int mobcount = 20; //rand()%30;
	for (int i = 0; i < mobcount; i++) {
		mob m;
		m.type = rand()%2 + 1;
		m.x = rand()%17 + 1;
		m.y = rand()%17 + 1;
		mobs.push_back(m);
	}
}


int handleevents() {
	SDL_Event event;

	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT:
			return 1;
		case SDL_WINDOWEVENT:
			// (int)event.window.event
			break;
		case SDL_KEYDOWN:

			// handle key press
			switch (event.key.keysym.sym) {
			case SDLK_ESCAPE:
				return 1;
			case SDLK_a:
			case SDLK_LEFT:
				playeraction(ACT_WEST);
				break;
			case SDLK_d:
			case SDLK_RIGHT:
				playeraction(ACT_EAST);
				break;
			case SDLK_w:
			case SDLK_UP:
				playeraction(ACT_NORTH);
				break;
			case SDLK_s:
			case SDLK_DOWN:
				playeraction(ACT_SOUTH);
				break;
			case SDLK_SPACE:
				playeraction(ACT_ACTION);
				break;
			}

			break;
		}  // end switch
	}  // end while

	return 0;
}


int playeraction(Action action) {
	switch (action) {
	case ACT_NONE:
		break;
	case ACT_WEST:
		if (!collide(playermob.x-1, playermob.y))
			playermob.x -= 1;
		break;
	case ACT_EAST:
		if (!collide(playermob.x+1, playermob.y))
			playermob.x += 1;
		break;
	case ACT_SOUTH:
		if (!collide(playermob.x, playermob.y+1))
			playermob.y += 1;
		break;
	case ACT_NORTH:
		if (!collide(playermob.x, playermob.y-1))
			playermob.y -= 1;
		break;
	case ACT_ACTION:
		break;
	}

	centercam();
	return 0;
}


int collide(int x, int y) {
	if (y < 0 || y >= map.size() || x < 0 || x >= map[0].size())
		return 1;
	if (map[y][x] < 0)
		return 1;
	for (auto m : mobs)
		if (m.x == x && m.y == y)
			return 1;
	return 0;
}


void centercam() {
	camera.x = playermob.x - 4;
	camera.y = playermob.y - 3;
}


void drawcard(int type, int x, int y) {
	static SDL_Rect cards[] = { spade, heart, club, diamond };

	SDL_Rect dst = cardback;
	dst.x = x;
	dst.y = y;
	SDL_RenderCopy(game::ren, sprites, &cardback, &dst);

	dst = cards[type];
	dst.x = x + 1;
	dst.y = y + 1;
	SDL_RenderCopy(game::ren, sprites, &cards[type], &dst);
}


void draw() {
	// cls
	SDL_SetRenderDrawColor(game::ren, 0, 0, 0, 255);
	SDL_RenderClear(game::ren);

	// game background
	SDL_SetRenderDrawColor(game::ren, 50, 50, 50, 255);
	SDL_Rect scr = { 0, 0, game::width, game::height };
	SDL_RenderFillRect(game::ren, &scr);

	SDL_Rect src, dst;

	// draw map
	dst = { 0, 0, 13, 13 };
	for (int y = 0; y < 10; y++) {
		if (camera.y+y < 0 || camera.y+y >= map.size())
			continue;
		dst.y = y * 12 - 4;

		for (int x = 0; x < 10; x++) {
			if (camera.x+x < 0 || camera.x+x >= map[0].size())
				continue;

			dst.x = x * 12 - 4;
			// draw block
			int tile = abs( map[camera.y+y][camera.x+x] );
			if (tile == 1)
				SDL_SetRenderDrawColor(game::ren, 0, 255, 0, 255);
			else if (tile == 2)
				SDL_SetRenderDrawColor(game::ren, 100, 100, 100, 255);
			else
				SDL_SetRenderDrawColor(game::ren, 0, 200, 0, 255);
			SDL_RenderFillRect(game::ren, &dst);
			// draw lines
			SDL_SetRenderDrawColor(game::ren, 255, 150, 150, 255);
			SDL_RenderDrawRect(game::ren, &dst);
		}
	}

	// main character
	src = man;
	src.x += 12 * animstate;
	dst = man;
	// dst.x = 12 * playermob.x - 4;
	// dst.y = 12 * playermob.y - 4;
	dst.x = 12 * 4 - 4;
	dst.y = 12 * 3 - 4;
	SDL_RenderCopy(game::ren, sprites, &src, &dst);

	// mobs
	for (auto m : mobs) {
		if (m.x < camera.x || m.x >= camera.x+10 || m.y < camera.y || m.y >= camera.y+10)
			continue;
		if (m.type == 1)
			src = scorpion;
		else if (m.type == 2)
			src = cake;
		src.x += 12 * animstate;
		dst = src;
		dst.x = 12 * (m.x - camera.x) - 4;
		dst.y = 12 * (m.y - camera.y) - 4;
		SDL_RenderCopy(game::ren, sprites, &src, &dst);
	}

	// draw parchment background
	dst = parchment;
	dst.x = 0;
	dst.y = 71;
	SDL_RenderCopy(game::ren, sprites, &parchment, &dst);

	// draw cards (staggered)
	// drawcard(0, 22, 74);
	// drawcard(1, 39, 78);
	// drawcard(2, 56, 81);
	// drawcard(3, 73, 80);

	// draw cards (inline)
	drawcard(0, 22, 77);
	drawcard(1, 39, 77);
	drawcard(2, 56, 77);
	drawcard(3, 73, 77);
}
