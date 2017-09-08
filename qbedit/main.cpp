#include "curs.h"
#include <iostream>

using namespace std;


int main(int argc, char** argv) {
	string fname;
	if (argc>=2) fname=argv[2];

	printf("hello\n");
	curs::init();

	init_pair(2, COLOR_BLACK,   COLOR_WHITE);
	mvprintw(0, 0, "  File  Edit  View  Search  Run  Help  ");
	mvchgat(0, 0, -1, 0, 2, NULL);
	move(1, 0);

	int doloop=1;
	while (doloop) {
		int k = getch();
		switch (k) {
		case 27:  nodelay(stdscr,true);  k=getch();  nodelay(stdscr,false);  break;
		case 'q':  doloop=0;  break;
		// case KEY_LEFT:  { int y,x; getyx(stdscr,y,x); move(y,x-1); }  break;
		// case KEY_RIGHT: { int y,x; getyx(stdscr,y,x); move(y,x+1); }  break;
		default:  mvprintw(1, 1, "key: %20s", keyname(k));  break;
		}
	}

	curs::quit();
}