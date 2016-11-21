#include <iostream>
#include <vector>
#include <sstream>
#include <ncurses.h>

#include "scr.h"

using namespace std;


// helpers
static void tolower(string& s) {
	for (auto& c : s)
		c = tolower(c);
}


namespace scr {

	enum COLORS {
		COL_BAR = 1,
		COL_TITLEBAR,
		COL_HEADBAR
	};

	static const int  MAP_W = 30,  MAP_H = 10,  HISTORY_MAX = 10;
	static int  width = 10,  height = 10;
	static WINDOW  *mainwin = NULL,  *mapwin = NULL;
	vector<string> history;
	string istr;


	void init() {
		initscr();
		noecho();
		keypad(stdscr, TRUE);
		//scrollok(stdscr, true);	
		start_color();
		init_pair(COL_BAR, COLOR_BLACK, COLOR_WHITE);
		init_pair(COL_TITLEBAR, COLOR_BLACK, COLOR_GREEN);
		init_pair(COL_HEADBAR, COLOR_BLACK, COLOR_BLUE);
		getmaxyx(stdscr, height, width);
		// make windows
		int main_top = 2;
		int main_h = (height - 1) - main_top;
		int main_w = (width - 1) - (MAP_W - 1) - 1;
		mainwin  = newwin(main_h, main_w, main_top, 1);
		scrollok(mainwin, true);
		mapwin   = newwin(MAP_H, MAP_W, main_top, main_w + 2);
		repaint();
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
		wprintw(mainwin, "start:\n");
		wrefresh(mainwin);
		end_input();
	}

	void repaint_map() {
		// box(mapwin, 0, 0);
		// wprintw(mapwin, "hello world 1234");
		// wmove(mapwin, 1, 0);
		int x = 9, y = 2;
		mvwprintw(mapwin, y++, x, "  ##  ##  ");
		mvwprintw(mapwin, y++, x, "####  ####");
		mvwprintw(mapwin, y++, x, "    @@    ");
		mvwprintw(mapwin, y++, x, "####  ####");
		mvwprintw(mapwin, y++, x, "  ##  ##  ");
		wrefresh(mapwin);
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
			tolower(istr);
			stringstream ss(istr);
			string s;
			vector<string> vs;
			while (ss >> s)  vs.push_back(s);
			history.push_back(s);
			if (history.size() > HISTORY_MAX)  history.erase(history.begin());
			if (run_command(vs))  return 1;
			istr = "";
		}
		wprintw(mainwin, "$> ");
		wrefresh(mainwin);
		return 0;
	}

	int run_command(vector<string>& vs) {
		if (vs.size() == 0)  
			return 0;
		else if (vs[0] == "quit" || vs[0] == "exit" || vs[0].substr(0, 2) == ":q")
			return 1;
		else if (vs[0] == "n")
			wprintw(mainwin, "you walked north.\n");
		else if (vs[0] == "s")
			wprintw(mainwin, "you walked south.\n");
		else if (vs[0] == "e")
			wprintw(mainwin, "you walked east.\n");
		else if (vs[0] == "w")
			wprintw(mainwin, "you walked west.\n");
		else
			wprintw(mainwin, "you typed: %s\n", istr.c_str());
		return 0;
	}

} // end scr