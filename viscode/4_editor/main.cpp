#include <ncurses.h>
#include <iostream>
#include <vector>

using namespace std;


const vector<pair<int, string>> headers={
	{ 1,  "[F1 namespace]" },
	{ 1+15, "[F2 struct]" },
	{ 1+15+12, "[F3 function]" },
	{ 1+15+12+14, "[F5 compile]" }
};
const int COL_HEAD=1, COL_HEAD_HL=2;
int hpos=2;


int paintwin() {
	clear();
	// header
	for (const auto& h : headers)
		mvprintw(0, h.first, "%s", h.second.c_str());
	mvchgat(0, 0, -1, 0, COL_HEAD, NULL);
	mvchgat(0, headers[hpos-1].first, headers[hpos-1].second.length(), 0, COL_HEAD_HL, NULL);
	refresh();
	// content
	switch (hpos) {
	case 1:
		mvprintw(1, 1, "def");
		break;
	}
	move(0, 0);
	return 0;
}


int main() {
	// init
	initscr();
	noecho();
	keypad(stdscr, TRUE);
	// nocbreak();
	// colors
	start_color();
	use_default_colors();
	init_pair(COL_HEAD, COLOR_BLUE, COLOR_WHITE);
	init_pair(COL_HEAD_HL, COLOR_WHITE, COLOR_BLUE);
	// cls
	clear();
	
	// main loop
	while (true) {
		paintwin();
		int c = getch();
		if      (c=='q') { break; }
		else if (c==KEY_F(1)) { hpos=1; }
		else if (c==KEY_F(2)) { hpos=2; }
		else if (c==KEY_F(3)) { hpos=3; }
		// else if (c==KEY_F(5)) { hpos=4; }
	}
	
	endwin();
	return 0;
}