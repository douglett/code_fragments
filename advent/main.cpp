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
int  movepos(char dir);
void pmap(char dir);
// member vars
Mob  guy("guy"), spider("spider");
GAME_MODE gamemode = MODE_NONE;
int running = 1;
int posx = 0, posy = 0;
vector<string> playermap, map = {
	"...  ",
	"  .  ",
	" ... ",
	" . . ",
	"   . ",
	".... "
};



int main() {
	cout << "hello" << endl;
	xd::screen::init();
	xd::screen::getinfo(&log::screenw, &log::screenh, NULL);
	xd::screen::keycb = keycb;

	switchmode(MODE_FIGHT);

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

template <class T>
static int indexOf(const T& needle, const vector<T>& haystack) {
	for (int i = 0; i < haystack.size(); i++)
		if (haystack[i] == needle)
			return i;
	return -1;
}


void switchmode(GAME_MODE mode) {
	switch (mode) {
	case 1:   log::title = { "fight!", 0x990000ff };  lprintf("a spider appears!");  break;
	case 2:   log::title = { "explore!", 0x000099ff };  lprintf("time to go...");  break;
	}
	gamemode = mode;
}


void parse(const string& input) {
	// set input
	stringstream ss(input);
	// get command
	string cmd;
	ss >> cmd;
	cmd = cstr::strtolower(cmd);
	// parse
	if (cmd == "quit")  running = 0;
	else if (cmd == "x") {
		if (gamemode == MODE_FIGHT)  switchmode(MODE_EXPLORE);
		else  lprintf("can't escape fate...");
	}
	else if (cmd == "stat" || cmd == "status") {
		lprintf("  hp %d/%d", guy.hp, guy.maxhp);
		lprintf("  atk %d  def %d  lck %d", guy.atk, guy.def, guy.lck);
	}
	else if (cmd == "l" || cmd == "look") {
		if (gamemode == MODE_FIGHT)  lprintf("look out! %s!", spider.name.c_str());
		else if (gamemode == MODE_EXPLORE)  lprintf("a nondescript room...");
	}
	// else if (cmd == "n" || cmd == "s" || cmd == "e" || cmd == "w") {
	else if (indexOf(cmd, { "n", "north", "s", "south", "e", "east", "w", "west" }) > -1) {
		if (gamemode == MODE_FIGHT)  lprintf("trapped!");
		if (gamemode == MODE_EXPLORE) {
			if (movepos(cmd[0]))  pmap(cmd[0]), parse("look");
			else  lprintf("oof. wall.");
		}
	}
	else if (cmd == "a" || cmd == "attack")  doattack();
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
		return 2;
	}
	return 0;
}


int movepos(char c) {
	switch (c) {
	case 'n':  if (posy > 0 && map[posy-1][posx] == '.')  { posy--;  return 1; }  break;
	case 's':  if (posy < map.size()-1 && map[posy+1][posx] == '.')  { posy++;  return 1; }  break;
	case 'w':  if (posx > 0 && map[posy][posx-1] == '.')  { posx--;  return 1; }  break;
	case 'e':  if (posy < map[posy].size()-1 && map[posy][posx+1] == '.')  { posx++;  return 1; }  break;
	}
	return 0;
}


void pmap(char c) {
	switch (c) {
	case 'n':
	case 's':
	case 'e':
	case 'w': ;
	}
}


//*** Mob class ***
Mob::Mob(std::string name) {
	this->name = name;
	hp = maxhp;
}