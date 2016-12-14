#include <iostream>
#include "xdlib/xdlib.h"
#include "game.h"

using namespace std;


int running = 1;


static void keycb(int key, int status) {
	if (!status)  return;
	switch (key) {
	case 27:    running = 0;  break;
	case 13:    if (log::input.length() > 0)  log::log(log::input), log::input = "";  break;
	case 127:
	case '\b':  log::delc();  break;
	default:
		if (key >= 32 && key <= 126)  log::inputc(key);
	}
	// printf("%d\n", key);
}


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