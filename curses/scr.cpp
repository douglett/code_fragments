#include <string>
#include <vector>
#include <ncurses.h>
#include "game.h"

using namespace std;


namespace scr {

	enum COLORS {
		COL_BAR = 1,
		COL_TITLEBAR,
		COL_HEADBAR
	};

	static const int  MAP_W = 30,  MAP_H = 9,  HISTORY_MAX = 10;
	static int  width = 10,  height = 10;
	static WINDOW  *mainwin = NULL,  *mapwin = NULL;
	static vector<string>  history;
	static string  istr;


	void init() {
		initscr();
		noecho();
		keypad(stdscr, TRUE);
		//scrollok(stdscr, true);	
		start_color();
		init_pair(COL_BAR,       COLOR_BLACK, COLOR_WHITE);
		init_pair(COL_TITLEBAR,  COLOR_BLACK, 8);
		init_pair(COL_HEADBAR,   COLOR_BLACK, COLOR_BLUE);
		getmaxyx(stdscr, height, width);
		// make windows
		int main_top = 2;
		int main_h = (height - 1) - main_top;
		int main_w = (width - 1) - (MAP_W - 1) - 3;
		mainwin  = newwin(main_h, main_w, main_top, 1);
		scrollok(mainwin, true);
		mapwin   = newwin(MAP_H, MAP_W, main_top, main_w + 2);
		repaint();
		loc::setup_thingums();
		interpret::run_command("l");
		end_input();
}

	static void paint_winborder(const WINDOW* win, const string& title = "") {
		int x, y, w, h;
		getbegyx(win, y, x);
		getmaxyx(win, h, w);
		attron(COLOR_PAIR(COL_HEADBAR));
			mvhline(y-1, x,   ' ', w);    // top
			mvprintw(y-1, x+1, ":%s", title.c_str());
		attroff(COLOR_PAIR(COL_HEADBAR));
		attron(COLOR_PAIR(COL_BAR));
			mvhline(y+h, x,   ' ', w);    // bottom
			mvvline(y-1, x-1, ' ', h+2);  // left
			mvvline(y-1, x+w, ' ', h+2);  // right
		attroff(COLOR_PAIR(COL_BAR));
	}

	void quit() {
		endwin();
	}

	void repaint() {
		// main window
		mvprintw(0, 2, ":game title");
		mvchgat(0, 0, -1, 0, COL_TITLEBAR, NULL);
		paint_winborder(mainwin);
		paint_winborder(mapwin, "map");
		refresh();
		// map sub-window
		repaint_map();
		// main sub-window
		// box(mainwin, 0, 0);
		wrefresh(mainwin);
	}

	void repaint_map() {
		int  w = 9,  h = 9;
		string map = loc::submap(w, h);
		for (int y = 0; y < h; y++)
			for (int x = 0; x < w; x++) {
				int col = 0;
				switch (map[y*w+x]) {
				case '.':  col = COL_BAR;  break;
				default:   col = COL_TITLEBAR;  break;
				}
				wattron  (mapwin, COLOR_PAIR(col));
				mvwprintw(mapwin, y, 6 + x*2, ( y == h/2 && x == w/2 ? "@@" : "  " ));
				wattroff (mapwin, COLOR_PAIR(col));
			}
		wrefresh(mapwin);
	}

	void println(const string& s) {
		wprintw(mainwin, "%s\n", s.c_str());
		wrefresh(mainwin);
	}

	int get_input() {
		int c = getch();
		switch (c) {
		case KEY_BACKSPACE:
		case 127:
		case '\b':  istr.pop_back();  wprintw(mainwin, "\b \b");  wrefresh(mainwin);  break;
		case '\n':  wprintw(mainwin, "\n");  wrefresh(mainwin);  if (end_input()) return 1;  break;
		case '\e':  return 1;
		default:
			if (c >= ' ' && c <= '~') {  // normal ascii range
				istr += c;
				wprintw(mainwin, "%c", c);
				wrefresh(mainwin);
			}
		}
		return 0;
	}

	int end_input() {
		if (istr.length()) {
			history.push_back(istr);
			if (history.size() > HISTORY_MAX)  history.erase(history.begin());
			if (interpret::run_command(istr))  return 1;
			istr = "";  // clear input state
		}
		// show command prompt
		wprintw(mainwin, "$> ");
		wrefresh(mainwin);
		return 0;
	}

} // end scr
