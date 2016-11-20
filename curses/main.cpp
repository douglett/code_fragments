#include <iostream>
#include <ncurses.h>

using namespace std;

int width = 10, height = 10;


int main() {
	cout << "hello world" << endl;
	initscr();
	start_color();
	noecho();
	//scrollok(stdscr, true);	
	init_pair(1, COLOR_BLACK, COLOR_GREEN);
	refresh();
	
	getmaxyx(stdscr, width, height);
	mvprintw(0, 2, "game title");
	mvchgat(0, 0, -1, 0, 1, NULL);
	refresh();

	WINDOW* mainwin = newwin(10, 20, 10, 20);
	box(mainwin, 0, 0);
	wprintw(mainwin, "hello world");
	wrefresh(mainwin);
	
	while (true) {
		int c = getch();
		if (c == 'q')  break;
	}

	endwin();
}
