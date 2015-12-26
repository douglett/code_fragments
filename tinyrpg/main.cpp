#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <map>
#include "libsrc/xcengine.h"
#include "globals.h"
// #include "tmap.h"
#include "lazymap.h"

using namespace std;



mob  create_mob(map<string, int>& mm);
int  get_action();
void draw();


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
const vector<string> mob_names = {
	"???",
	"scorp",
	"cakey"
};


// main.cpp globals
int showmenu = 1;
int animtt = 0, animstate = 0;
SDL_Rect camera = { 0, 0, 10, 10 };
SDL_Texture* sprites = NULL;
vector<string> gmap;
vector<mob> gmobs;
mob playermob;
vector <string> combat_log;
vector<gtext> gtexts;

namespace gamestate {
	int gamemode = MODE_GAME;
	int movecount = 0;
}



int main() {
	cout << "hello world" << endl;
	if (game::init())
		return 1;

	// sprite images
	sprites = texture::get("images")->tex;

	// build maps
	lazymap::buildmap(6000);
	srand(time(NULL));
	gmap = lazymap::gmap;
	auto& mobcache = lazymap::gmobs;

	// dump map in console
	for (auto& s : gmap)
		cout << s << endl;

	// make main player
	camera.w = ceil(game::width/12.0);
	camera.h = ceil(game::height/12.0);
	playermob.hp = playermob.maxhp = 20;
	playermob.x = 4;
	playermob.y = 3;
	playermob.name = "player";
	menu::cards.push_back(menu::CARD_HEART);
	// see if the map creator sent us some start coordinates
	if (mobcache.size() > 0 && mobcache[0]["type"] == -1) {
		playermob.x = mobcache[0]["x"];
		playermob.y = mobcache[0]["y"];
		mobcache.erase(mobcache.begin());
	}
	// recenter cam
	centercam();

	// make mobs
	for (auto& mm : mobcache)
		gmobs.push_back(create_mob(mm));

	gamestate::movecount = 0;

	while (true) {
		animtt++;
		if (animtt % 30 == 0) {
			animtt = 0;
			animstate = !animstate;
		}

		draw();
		SDL_RenderPresent(game::ren);
		game::waitscreen();
		
		// take player action
		int action = get_action();
		int action_performed = 0;
		if (action == action::ACT_KILL) {
			break;
		} else if (gamestate::gamemode == gamestate::MODE_GAME) {
			action_performed = action::playeraction(action);
		} else if (gamestate::gamemode == gamestate::MODE_GAMEMENU) {
			action_performed = menu::playeraction(action);
		}

		// give player a card
		if (action_performed) {
			gamestate::movecount++;
			if (gamestate::movecount % menu::CARD_DRAW_RATE == 0) {
				menu::givecard();
			}
		}
		
		// kill player
		if (playermob.hp <= 0) {
			cout << "you died" << endl;
			break;
		}
	}

	game::quit();
}


stringstream& ss(int reset) {
	static stringstream strm;
	if (reset)
		strm.str(""), strm.clear();
	return strm;
}


mob create_mob(map<string, int>& mm) {
	mob m;
	m.x = mm["x"];
	m.y = mm["y"];
	m.type = mm["type"];
	m.name = mob_names[m.type];
	return m;
}


gtext create_gtext(int x, int y, string s, int type) {
	gtext g;
	g.x = x;
	g.y = y;
	g.s = s;
	g.type = type;
	return g;
}



int get_action_inner() {
	SDL_Event event;

	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		 case SDL_QUIT:
			return action::ACT_KILL;
		 case SDL_WINDOWEVENT:
			// (int)event.window.event
			break;
		 case SDL_KEYDOWN:
			// handle key press
			switch (event.key.keysym.sym) {
			 case SDLK_ESCAPE:
				return action::ACT_KILL;
			 case SDLK_LEFT:
				return action::ACT_WEST;
			 case SDLK_RIGHT:
				return action::ACT_EAST;
			 case SDLK_UP:
				return action::ACT_NORTH;
			 case SDLK_DOWN:
				return action::ACT_SOUTH;
			 case SDLK_SPACE:
			 case SDLK_z:
				return action::ACT_ACTION;
			 case SDLK_x:
				return action::ACT_CANCEL;
			 case SDLK_s:
				return action::ACT_MENU;
			 case SDLK_a:
				return action::ACT_SELECT;
			}
			break;
		}  // end switch
	}  // end while

	return action::ACT_NONE;
}

int get_action() {
	int event = get_action_inner();
	if (game::clearevents())
		return action::ACT_KILL;
	return event;
}



void cleardead() {
	for (int i = 0; i < gmobs.size(); i++)
		if (gmobs[i].hp <= 0) {
			ss(1) << gmobs[i].name << " died";
			combatlog(ss().str());
			gmobs.erase(gmobs.begin()+i);
			i--;
		}
}
void centercam() {
	camera.x = playermob.x - floor((camera.w-0.5)/2);
	camera.y = playermob.y - floor((camera.h-0.5)/2);
}
void combatlog(const string& s) {
	combat_log.push_back(s);
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


// all draw actions
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
		if (camera.y+y < 0 || camera.y+y >= gmap.size())
			continue;
		dst.y = y * 12 + offsety;

		for (int x = 0; x < camera.w; x++) {
			if (camera.x+x < 0 || camera.x+x >= gmap[0].size())
				continue;

			dst.x = x * 12 + offsetx;
			// draw block
			switch ( gmap[camera.y+y][camera.x+x] ) {
				case ' ':
					continue;  // use background
				case '#':
					SDL_SetRenderDrawColor(game::ren, 100, 100, 100, 255);
					break;
				case '.':
					SDL_SetRenderDrawColor(game::ren, 0, 200, 0, 255);
					break;
				case '/':
					SDL_SetRenderDrawColor(game::ren, 160, 100, 100, 255);
					break;
				default:
					SDL_SetRenderDrawColor(game::ren, 255, 0, 255, 255);  // unknown - hot pink
			}
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
	for (auto m : gmobs) {
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
		if (g.type == 1)
			game::qbcolor(0, 240, 0);
		else
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
		for (int i = 0; i < menu::cards.size(); i++)
			drawcard(menu::cards[i], parchment_pos.x+22+(i*17), parchment_pos.y+6);
		
		// menu markers
		if (gamestate::gamemode == gamestate::MODE_GAMEMENU) {
			int x = parchment_pos.x + 22 + 5 + (menu::handpos*17);
			int y = parchment_pos.y + 26;
			string s = string()+char(24);
			game::qbcolor(70, 70, 70);
			game::qbprint(x+1, y+1, s);
			game::qbcolor(255, 0, 0);
			game::qbprint(x, y, s);
		}

		// background
		SDL_Rect textbox = { 0, 1, 41, 28 };
		textbox.x = game::width - textbox.w - 1;
		SDL_SetRenderDrawColor(game::ren, 0, 0, 0, 150);
		SDL_RenderFillRect(game::ren, &textbox);

		// HP text
		ss(1) 	<< setfill('0') << setw(2) << playermob.hp << "/"
				<< setfill('0') << setw(2) << playermob.maxhp;
		game::qbcolor(0, 0, 0);
		game::qbprint(textbox.x+2, textbox.y+2, ss().str());
		game::qbcolor(0, 200, 0);
		game::qbprint(textbox.x+1, textbox.y+1, ss().str());

		// ATK text
		ss(1) << "atk " << playermob.atk;
		game::qbcolor(0, 0, 0);
		game::qbprint(textbox.x+2, textbox.y+11, ss().str());
		game::qbcolor(230, 230, 0);
		game::qbprint(textbox.x+1, textbox.y+10, ss().str());

		// DEF text
		ss(1) << "def " << playermob.def;
		game::qbcolor(0, 0, 0);
		game::qbprint(textbox.x+2, textbox.y+20, ss().str());
		game::qbcolor(230, 230, 0);
		game::qbprint(textbox.x+1, textbox.y+19, ss().str());

		// moves
		ss(1) << gamestate::movecount;
		game::qbcolor(0, 0, 0);
		game::qbprint(textbox.x+2, textbox.y+30, ss().str());
		game::qbcolor(230, 230, 0);
		game::qbprint(textbox.x+1, textbox.y+29, ss().str());

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
		ss(1) << playermob.hp << "/" << playermob.maxhp;
		string s = ss().str();

		// background
		SDL_Rect textbox = { 0, 1, int(s.length())*8 + 1, 10 };
		textbox.x = game::width - textbox.w - 1;
		SDL_SetRenderDrawColor(game::ren, 0, 0, 0, 150);
		SDL_RenderFillRect(game::ren, &textbox);

		game::qbcolor(0, 0, 0);
		game::qbprint(textbox.x+2, textbox.y+2, s);
		game::qbcolor(0, 200, 0);
		game::qbprint(textbox.x+1, textbox.y+1, s);
	}
}
