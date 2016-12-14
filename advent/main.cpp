#include <iostream>
#include <sstream>
#include "xdlib/xdlib.h"
#include "game.h"

using namespace std;


// functions
static void keycb(int key, int status);
void parse();
void showstatus();
int  doattack();
// struct
struct blob {
	int maxhp  = 10,
		hp     = 0,
		atk    = 2,
		def    = 0,
		lck    = 50;
	string name;
	blob(string name) {
		this->name = name;
		hp = maxhp;
	}
};
// member vars
blob guy("guy");
blob spider("spider");
int running = 1;



int main() {
	cout << "hello" << endl;
	xd::screen::init();
	xd::screen::getinfo(&log::screenw, &log::screenh, NULL);
	xd::screen::keycb = keycb;

	lprintf("hello");
	lprintf("world");
	lprintf("lol");

	while (running) {
		log::paint();  // paints if dirty
		if (xd::screen::paint())  break;
	}

	xd::screen::quit();
}


static void keycb(int key, int status) {
	if (!status)  return;
	switch (key) {
	case 27:    log::input = "";  log::dirtyi = 1;  break;
	case 13:    if (log::input.length() > 0)  parse();  break;
	case 127:
	case '\b':  log::delc();  break;
	default:
		if (key >= 32 && key <= 126)  log::inputc(key);
	}
	// printf("%d\n", key);
}


void parse() {
	// set input
	stringstream ss(log::input);
	// get command
	string cmd;
	ss >> cmd;
	cmd = cstr::strtolower(cmd);
	// parse
	if (cmd == "quit")  running = 0;
	else if (cmd == "s" || cmd == "status")  showstatus();
	else if (cmd == "a" || cmd == "attack")  doattack();
	else  lprintf("?"), log::input.c_str();  // lprintf("[? %s]  %s", cmd.c_str(), log::input.c_str());
	// reset input
	log::input = "", log::dirtyi = 1;
}


int doattack() {
	if (spider.hp <= 0) {
		lprintf("spider is dead");
		return 1;
	}
	int dmg = guy.atk + ( rand()%100 < guy.lck ? guy.atk/2 : 0 );
	spider.hp -= dmg;
	lprintf("attacking spider [%d dmg]", dmg);
	if (spider.hp <= 0) {
		lprintf("spider dies");
		return 1;
	}
	dmg = spider.atk + ( rand()%100 < spider.lck ? spider.atk/2 : 0 );
	guy.hp -= dmg;
	lprintf("spider attacks [%d dmg]", dmg);
	if (guy.hp <= 0) {
		lprintf("you die");
		return 2;
	}
	return 0;
}


void showstatus() {
	lprintf("  [hp %d/%d]", guy.hp, guy.maxhp);
	lprintf("  [atk %d] [def %d]", guy.atk, guy.def);
	lprintf("  [lck %d]", guy.lck);
}