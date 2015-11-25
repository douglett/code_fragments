#include <iostream>
#include <sstream>
#include <iomanip>
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

const vector<string> mob_names = {
	"???",
	"scorp",
	"cakey"
};

vector <string> combat_log;

class mob {
public:
	int x = 0;
	int y = 0;
	int type = 0;
	int hp = 3;
	int maxhp = 3;
	int atk = 1;
	int def = 0;
	const string& name() {
		return mob_names[type];
	}
};

class gtext {
public:
	int x = 0;
	int y = 0;
	string s;
};

void draw();
void createmap();
int  handleevents();
int  playeraction(Action action);
int  playercollide(int x, int y);
void playerattack(mob& m);
void allenemyactions();
void enemyaction(mob& m);
int  mobcollide(mob& currentmob, int offsetx, int offsety);
void cleartexts();
void centercam();
void combatlog(const string& s);

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
SDL_Texture* sprites = NULL;
mob playermob;
SDL_Rect camera = { 0, 0, 10, 10 };
vector<mob> mobs;
vector<gtext> gtexts;
int showmenu = 0;
int animtt = 0, animstate = 0;
stringstream ss;



int main() {
	cout << "hello world" << endl;
	if (game::init())
		return 1;

	createmap();
	playermob.x = 4;
	playermob.y = 3;
	camera.w = ceil(game::width/12.0);
	camera.h = ceil(game::height/12.0);
	// camera.w = 14;
	// camera.h = 10;
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

	// int seed = time(NULL);
	int seed = 5000;

	// create map
	srand(seed);
	for (int y = 0; y < 20; y++) {
		map.push_back(vector<int>());
		for (int x = 0; x < 20; x++) {
			if (x == 0 || y == 0 || x == 19 || y == 19)
				map[y].push_back( -2 );
			else if (rand()%15 == 0)
				map[y].push_back( -2 );
			else
				map[y].push_back( rand()%2 );
		}
	}

	// make mobs
	srand(seed);
	int mobcount = 15 + rand()%15;
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
			case SDLK_LEFT:
				playeraction(ACT_WEST);
				break;
			case SDLK_RIGHT:
				playeraction(ACT_EAST);
				break;
			case SDLK_UP:
				playeraction(ACT_NORTH);
				break;
			case SDLK_DOWN:
				playeraction(ACT_SOUTH);
				break;
			case SDLK_SPACE:
				playeraction(ACT_ACTION);
				break;
			case SDLK_s:
				showmenu = !showmenu;
				break;
			}

			break;
		}  // end switch
	}  // end while

	return 0;
}


int playeraction(Action action) {
	int doaction = 0;

	switch (action) {
	case ACT_NONE:
		break;
	case ACT_WEST:
		if (!playercollide(playermob.x-1, playermob.y))
			playermob.x -= 1, doaction = 1;
		break;
	case ACT_EAST:
		if (!playercollide(playermob.x+1, playermob.y))
			playermob.x += 1, doaction = 1;
		break;
	case ACT_SOUTH:
		if (!playercollide(playermob.x, playermob.y+1))
			playermob.y += 1, doaction = 1;
		break;
	case ACT_NORTH:
		if (!playercollide(playermob.x, playermob.y-1))
			playermob.y -= 1, doaction = 1;
		break;
	case ACT_ACTION:
		break;
	}

	if (doaction) {
		cleartexts();
		allenemyactions();
	}
	centercam();
	return 0;
}


int playercollide(int x, int y) {
	if (y < 0 || y >= map.size() || x < 0 || x >= map[0].size())
		return 1;
	if (map[y][x] < 0)
		return 1;
	for (auto &m : mobs)
		if (m.x == x && m.y == y) {
			playerattack(m);
			return 1;
		}
	return 0;
}


void playerattack(mob& m) {
	// do attack
	int atk = playermob.atk - m.def;
	m.hp -= atk;

	// display attack
	cleartexts();
	ss.str(""), ss.clear();
	ss << atk;
	gtext g;
		g.x = m.x;
		g.y = m.y;
		g.s = ss.str();
	gtexts.push_back(g);

	// add player log
	ss.str(""), ss.clear();
	ss << m.name() << " < -" << atk;
	combatlog(ss.str());

	// do counterattack

	// erase mob
	if (m.hp <= 0) {
		for (int i = 0; i < mobs.size(); i++) {
			if (&mobs[i] == &m) {
				cout << "killed mob: " << i << endl;
				mobs.erase(mobs.begin()+i);
				break;
			}
		}
	}
}


void combatlog(const string& s) {
	combat_log.push_back(s);
}


void allenemyactions() {
	for (auto &m : mobs) {
		int dist = sqrt(pow(playermob.x - m.x, 2) + pow(playermob.y - m.y, 2));
		// printf("%d %d %d\n", (playermob.x - m.x), (playermob.y - m.y), dist);
		if (dist <= 3)
			enemyaction(m);
	}
}


void enemyaction(mob& m) {
	int diffx = playermob.x - m.x;
	int diffy = playermob.y - m.y;
	// cout << diffx << " " << diffy << endl;
	if (diffy <= -1 && !mobcollide(m, 0, -1)) {
		m.y -= 1;
	} else if (diffy >= 1 && !mobcollide(m, 0, 1)) {
		m.y += 1;
	} else if (diffx <= -1 && !mobcollide(m, -1, 0)) {
		m.x -= 1;
	} else if (diffx >= 1 && !mobcollide(m, 1, 0)) {
		m.x += 1;
	}
}


int mobcollide(mob& currentmob, int offsetx, int offsety) {
	int x = currentmob.x + offsetx;
	int y = currentmob.y + offsety;
	if (y < 0 || y >= map.size() || x < 0 || x >= map[0].size())
		return 1;
	if (map[y][x] < 0)
		return 1;
	for (auto &m : mobs)
		if (m.x == x && m.y == y)
			return 1;
	if (x == playermob.x && y == playermob.y)
		return 2;
	return 0;
}


void cleartexts() {
	gtexts.erase(gtexts.begin(), gtexts.end());
}


void centercam() {
	camera.x = playermob.x - floor((camera.w-0.5)/2);
	camera.y = playermob.y - floor((camera.h-0.5)/2);
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
	// SDL_SetRenderDrawColor(game::ren, 50, 50, 50, 255);
	// SDL_Rect scr = { 0, 0, game::width, game::height };
	// SDL_RenderFillRect(game::ren, &scr);

	SDL_Rect src, dst;

	const int 
		offsety = 0,
		offsetx = 0;

	// draw map
	dst = { 0, 0, 13, 13 };
	for (int y = 0; y < camera.h; y++) {
		if (camera.y+y < 0 || camera.y+y >= map.size())
			continue;
		dst.y = y * 12 + offsety;

		for (int x = 0; x < camera.w; x++) {
			if (camera.x+x < 0 || camera.x+x >= map[0].size())
				continue;

			dst.x = x * 12 + offsetx;
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
			// SDL_SetRenderDrawColor(game::ren, 255, 150, 150, 255);
			// SDL_RenderDrawRect(game::ren, &dst);
		}
	}

	// main character
	src = man;
	src.x += 12 * animstate;
	dst = man;
	// dst.x = 12 * playermob.x - 4;
	// dst.y = 12 * playermob.y - 4;
	// dst.x = 12 * 4 - 4;
	// dst.y = 12 * 3 - 4;
	dst.x = 12 * (playermob.x - camera.x) + offsetx;
	dst.y = 12 * (playermob.y - camera.y) + offsety;
	SDL_RenderCopy(game::ren, sprites, &src, &dst);

	// mobs
	for (auto m : mobs) {
		if (m.x < camera.x || m.x >= camera.x+camera.w || m.y < camera.y || m.y >= camera.y+camera.h)
			continue;
		if (m.type == 1)
			src = scorpion;
		else if (m.type == 2)
			src = cake;
		src.x += 12 * animstate;
		dst = src;
		dst.x = 12 * (m.x - camera.x) + offsetx;
		dst.y = 12 * (m.y - camera.y) + offsety;
		SDL_RenderCopy(game::ren, sprites, &src, &dst);
	}

	// attack text
	for (auto g : gtexts) {
		dst.x = 12 * (g.x - camera.x) + offsetx + 1;
		dst.y = 12 * (g.y - camera.y) + offsety - 2;
		game::qbcolor(0, 0, 0);
		game::qbprint(dst.x+1, dst.y+1, g.s);
		game::qbcolor(200, 0, 0);
		game::qbprint(dst.x, dst.y, g.s);
	}

	// draw large info
	if (showmenu) {
		// draw parchment background
		auto parchment_pos = parchment;
		// parchment_pos.x = (game::width - parchment_pos.w) - 1;
		// parchment_pos.y = game::height - parchment_pos.h - 1;
		parchment_pos.x = 1;
		parchment_pos.y = 1;
		SDL_RenderCopy(game::ren, sprites, &parchment, &parchment_pos);

		// draw cards (staggered)
		// drawcard(0, 22, 74);
		// drawcard(1, 39, 78);
		// drawcard(2, 56, 81);
		// drawcard(3, 73, 80);

		// draw cards (inline)
		drawcard(0, parchment_pos.x+22, parchment_pos.y+6);
		drawcard(1, parchment_pos.x+39, parchment_pos.y+6);
		drawcard(2, parchment_pos.x+56, parchment_pos.y+6);
		drawcard(3, parchment_pos.x+73, parchment_pos.y+6);

		// background
		SDL_Rect textbox = { 0, 1, 41, 28 };
		textbox.x = game::width - textbox.w - 1;
		SDL_SetRenderDrawColor(game::ren, 0, 0, 0, 150);
		SDL_RenderFillRect(game::ren, &textbox);

		// HP text
		ss.str(""), ss.clear();
		ss  << setfill('0') << setw(2) << playermob.hp << "/" 
			<< setfill('0') << setw(2) << playermob.maxhp;
		game::qbcolor(0, 0, 0);
		game::qbprint(textbox.x+2, textbox.y+2, ss.str());
		game::qbcolor(0, 200, 0);
		game::qbprint(textbox.x+1, textbox.y+1, ss.str());

		// ATK text
		ss.str(""), ss.clear();
		ss << "atk " << playermob.atk;
		game::qbcolor(0, 0, 0);
		game::qbprint(textbox.x+2, textbox.y+11, ss.str());
		game::qbcolor(230, 230, 0);
		game::qbprint(textbox.x+1, textbox.y+10, ss.str());

		// DEF text
		ss.str(""), ss.clear();
		ss << "def " << playermob.def;
		game::qbcolor(0, 0, 0);
		game::qbprint(textbox.x+2, textbox.y+20, ss.str());
		game::qbcolor(230, 230, 0);
		game::qbprint(textbox.x+1, textbox.y+19, ss.str());

		// combat log
		for (int i = 0; i < 5; i++) {
			if (i >= combat_log.size())
				break;
			int x = 1;
			int y = game::height - 10 - 8*i;
			auto s = *(combat_log.end() - (1+i));
			game::qbcolor(0, 0, 0);
			game::qbprint(x+1, y+1, s);
			// game::qbcolor(255, 255, 255);
			game::qbcolor(230, 230, 0);
			game::qbprint(x, y, s);
		}
	} 
	// draw small info
	else {
		// HP text
		ss.str(""), ss.clear();
		ss << playermob.hp << "/" << playermob.maxhp;
		string s = ss.str();

		// background
		SDL_Rect textbox = { 0, 1, int(s.length())*8 + 1, 10 };
		textbox.x = game::width - textbox.w - 1;
		SDL_SetRenderDrawColor(game::ren, 0, 0, 0, 150);
		SDL_RenderFillRect(game::ren, &textbox);

		game::qbcolor(0, 0, 0);
		game::qbprint(textbox.x+2, textbox.y+2, ss.str());
		game::qbcolor(0, 200, 0);
		game::qbprint(textbox.x+1, textbox.y+1, ss.str());
	}
}
