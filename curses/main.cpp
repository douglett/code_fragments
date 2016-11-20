#include <iostream>
#include <ncurses.h>
#include "scr.h"

using namespace std;


namespace scr {

	enum COLORS {
		COL_BAR = 1,
		COL_TITLEBAR,
		COL_HEADBAR
	};

	static const int  MAP_W = 30,  MAP_H = 10;
	static int  width = 10,  height = 10;
	static WINDOW  *mainwin = NULL,  *mapwin = NULL;
	string istr;


	void init() {
		initscr();
		start_color();
		noecho();
		//scrollok(stdscr, true);	
		init_pair(COL_BAR, COLOR_BLACK, COLOR_WHITE);
		init_pair(COL_TITLEBAR, COLOR_BLACK, COLOR_GREEN);
		init_pair(COL_HEADBAR, COLOR_BLACK, COLOR_BLUE);
		getmaxyx(stdscr, height, width);
		// make windows
		int main_h = height - 3;
		int main_w = width - MAP_W - 3;
		int main_top = 2;
		mainwin  = newwin(main_h, main_w, main_top, 1);
		mapwin   = newwin(MAP_H, MAP_W, main_top, main_w + 2);
		repaint();
	}

	static void paint_winborder(const WINDOW* win, const string& title = "") {
		int x, y, w, h;
		getbegyx(win, y, x);
		getmaxyx(win, h, w);
		attron(COLOR_PAIR(COL_HEADBAR));
			mvhline(y-1, x,   ' ', w);    // top
			mvprintw(y-1, x+1, "%s", title.c_str());
		attroff(COLOR_PAIR(COL_HEADBAR));
		attron(COLOR_PAIR(COL_BAR));
			mvhline(y+h, x,   ' ', w);    // bottom
			mvvline(y-1, x-1, ' ', h+2);  // left
			mvvline(y-1, x+w, ' ', h+2);  // right
		attroff(COLOR_PAIR(COL_BAR));
	}

	void repaint() {
		// main window
		mvprintw(0, 2, "game title");
		mvchgat(0, 0, -1, 0, 1, NULL);
		paint_winborder(mainwin);
		paint_winborder(mapwin);
		refresh();
		// map sub-window
		// box(mapwin, 0, 0);
		wprintw(mapwin, "hello world 1234");
		wrefresh(mapwin);
		// main sub-window
		// box(mainwin, 0, 0);
		wprintw(mainwin, "start:\n");
		wrefresh(mainwin);
	}

	int docmd() {
		wprintw(mainwin, "you typed: %s\n", istr.c_str());
		wrefresh(mainwin);
		istr = "";
	}

	int dotype() {
		int c = getch();
		if ((c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == ' ') {
			istr += c;
			wprintw(mainwin, "%c", c);
			wrefresh(mainwin);
		}
		switch (c) {
		case 127:
		case '\b':  istr.pop_back();  wprintw(mainwin, "\b \b");  wrefresh(mainwin);  break;
		case '\n':  wprintw(mainwin, "\n");  wrefresh(mainwin);  docmd();  break;
		case '\e':  return 1;
		}
		return 0;
	}

	void quit() {
		endwin();
	}

} // end scr



int main() {
	cout << "hello world" << endl;
	scr::init();

	while (true) {
		if (scr::dotype())  break;
	}

	scr::quit();
}
