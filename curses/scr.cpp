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

	static const int  MAP_W = 30,  MAP_H = 10,  HISTORY_MAX = 10;
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

	void println(const string& s) {
		wprintw(mainwin, "%s\n", s.c_str());
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