#include <ncurses.h>
#include <iostream>
#include <string>
#include <vector>
#include <cmath>

using namespace std;


const int 
	OP_EOT=4,
	OP_QUIT=-2,
	OP_BACK=-1;

struct M_Structs {
	string name;
};
vector<M_Structs> MSlist={
	{ .name="A_struct" },
	{ .name="B_struct" },
	{ .name="C_struct" }
};


int l_menu(const vector<string>& mlist) {
	int page=0;
	// display loop
	while (true) {
		clear();
		move(1, 0);
		printw("structs  [page %d]\n", page+1);
		int pstart = page * 10;
		for (int i=0; i<10; i++) {
			if (pstart+i >= mlist.size())  break;
			printw("  %d) %s\n", (i+1)%10, mlist[pstart+i].c_str());
		}
		printw("\n(A) UP  (Z) DOWN  (B) BACK  (Q) QUIT\n");
		// keys
		int c=getch();
		switch (c) {
		case 'q':  return -2;
		case 'b':  return -1;
		case 'a':  if (page > 0) page--;  break;
		case 'z':  if ((page+1)*10 < mlist.size()) page++;  break;
		}
		if (c>='1' && c<='9')  return c-'1';
		if (c=='0')  return 0;
	}
	return -2;
}


// int l_struct() {
// 	return 0;
// }
int l_structmenu() {
	vector<string> mlist={
		"blahA", "blahB", "blahC", "blahD", "blahE",
		"blahF", "blahG", "blahH", "blahI", "blahJ",
		"blahK", "blahL", "blahM", "blahN", "blahO",
	};
	while (true) {
		int opt = l_menu(mlist);
		if (opt < 0) { return opt; }
	}
}
int l_mainmenu() {
	vector<string> mlist={ "structs", "functions" };
	while (true) {
		int opt = l_menu(mlist);
		if      (opt < 0)  { return opt; }
		else if (opt == 0) { if (l_structmenu() == OP_QUIT) return OP_QUIT; }
	}
}

int l_showinput() {
	for (int c=0; c!=OP_EOT; c=getch()) {
		move(1, 0);
		printw("key: %c  %d   ", c);
	}
	return 0;
}


int main() {
	initscr();
	noecho();
	keypad(stdscr, TRUE);
	// nocbreak();
	clear();  // cls
	
	l_mainmenu();
	// l_structmenu();
	
	endwin();
	return 0;
}
