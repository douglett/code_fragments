#include <iostream>
#include <sstream>
#include "xdlib/xdlib.h"
#include "game.h"

using namespace std;


// functions
static void keycb(int key, int status);
void switchmode(GAME_MODE mode);
void parse(const std::string& input);
int  doattack();
// member vars
Mob  guy("guy"), spider("spider");
GAME_MODE gamemode = MODE_NONE;
int running = 1;



//*** main stuff ***
int main() {
	cout << "hello" << endl;
	srand(time(NULL));
	xd::screen::init();
	xd::screen::getinfo(&log::screenw, &log::screenh, NULL);
	xd::screen::keycb = keycb;
	// move to first map
	map::nextmap();
	switchmode(MODE_EXPLORE);
	lprintf("(press h for help)");
	// guy.hp = 1;

	while (running) {
		log::paint();  // paints if dirty
		if (xd::screen::paint())  break;
	}

	xd::screen::quit();
}


static void keycb(int key, int status) {
	if (!status)  return;
	switch (key) {
	case 27:    log::clearc();  break;
	case 13:    if (log::input.length() > 0)  parse(log::input), log::clearc();  break;
	case 127:
	case '\b':  log::delc();  break;
	default:
		if (key >= 32 && key <= 126)  log::inputc(key);
	}
	// printf("%d\n", key);
}


void switchmode(GAME_MODE mode) {
	gamemode = mode;
	switch (mode) {
	case MODE_FIGHT:    log::title = { "fight!", 0x990000ff };  spider = Mob("spider");  lprintf("a %s appears!", spider.name.c_str());  break;
	case MODE_EXPLORE:  log::title = { "explore!", 0x000099ff };  lprintf("time to go...");  parse("l");  break;
	default:            log::title = { "...", 0 };  break;
	}
}


void parse(const string& input) {
	// set input
	stringstream ss(input);
	// get command
	string cmd;
	ss >> cmd;
	cmd = util::strtolower(cmd);
	// parse
	if (cmd == "qu" || cmd == "quit")  running = 0;
	else if (cmd == "h" || cmd == "help") {
		lprintf(" (qu)it");
		lprintf(" (h)elp");
		lprintf(" (st)atus");
		lprintf(" (l)ook");
		lprintf(" (m)ap");
		lprintf(" (a)ttack");
		lprintf(" (n) (s) (e) (w)");
	}
	else if (cmd == "x") {
		if       (gamemode == MODE_FIGHT)  switchmode(MODE_EXPLORE);
		else if  (gamemode == MODE_NONE)   running = 0;
		else     lprintf("can't escape fate...");
	}
	else if (cmd == "st" || cmd == "stat" || cmd == "status") {
		lprintf("  hp %d/%d", guy.hp, guy.maxhp);
		lprintf("  atk %d  def %d  lck %d", guy.atk, guy.def, guy.lck);
	}
	else if (cmd == "l" || cmd == "look") {
		if    (gamemode == MODE_FIGHT)  lprintf("look out! %s!", spider.name.c_str());
		else  lprintf("(%s)  %s", map::exitstr().c_str(), map::roomdesc().c_str());
	}
	else if (util::indexOf(cmd, { "n", "north", "s", "south", "e", "east", "w", "west", "d", "down" }) > -1) {
		if  (gamemode == MODE_EXPLORE) {
			if  (map::movepos(cmd[0])) {
				parse("look");
				if (map::current() == 'x')  switchmode(MODE_FIGHT);
			}
			else  lprintf("oof. wall.");
		}
		else  lprintf("trapped!");
	}
	else if (cmd == "m" || cmd == "map") {
		if    (gamemode == MODE_EXPLORE)  map::showmap();
		else  lprintf("no...");
	}
	else if (cmd == "a" || cmd == "attack") {
		if    (gamemode == MODE_FIGHT) { if (doattack() == 2)  switchmode(MODE_NONE); }
		else  lprintf("swish.");
	}
	else  lprintf("wat?"), log::input.c_str();
}


int doattack() {
	if (gamemode != MODE_FIGHT) {
		lprintf("nothing to fight");
		return 0;
	}
	if (spider.hp <= 0) {
		lprintf("%s is dead", spider.name.c_str());
		return 1;
	}
	int dmg = guy.atk + ( rand()%100 < guy.lck ? guy.atk/2 : 0 );
	spider.hp -= dmg;
	lprintf("attacking %s [%d dmg]", spider.name.c_str(), dmg);
	if (spider.hp <= 0) {
		lprintf("%s dies", spider.name.c_str());
		lprintf("(x to exit)");
		return 1;
	}
	dmg = spider.atk + ( rand()%100 < spider.lck ? spider.atk/2 : 0 );
	guy.hp -= dmg;
	lprintf("%s attacks [%d dmg]", spider.name.c_str(), dmg);
	if (guy.hp <= 0) {
		lprintf("you die");
		lprintf("(x to exit)");
		return 2;
	}
	return 0;
}