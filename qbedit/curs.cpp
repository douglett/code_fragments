#include "curs.h"
#include <ncurses.h>
#include <string>

using namespace std;


int curs::init() {
	initscr();
	noecho();
	keypad(stdscr, true);
	start_color();
	use_default_colors();

	init_pair(1, COLOR_WHITE, COLOR_BLUE);
	bkgdset(COLOR_PAIR(1));  // background color
	clear();

	// attron(COLOR_PAIR(2));
	// mvprintw(1, 1, "hello world\n");
	// mvprintw(height()-1, width()-2, "abc");
	// refresh();
	// getch();
	return 0;
}
int curs::quit() {
	endwin();
	return 0;
}
int curs::pause() {
	return 0;
}
int curs::width()  { int w, h;  getmaxyx(stdscr, h, w);  return w; }
int curs::height() { int w, h;  getmaxyx(stdscr, h, w);  return h; }