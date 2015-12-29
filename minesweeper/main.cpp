#include <iostream>
#include <vector>
#include <ncurses.h>

using namespace std;


// definitions
int  redraw();
int  make_board();
char gettile(int t);
void setcolor(char c);
int  uncover(int xpos, int ypos);
int  flag(int xpos, int ypos);
int  count_near(int xpos, int ypos);
int  game_result();

// enums
enum TILES {
	TILE_EMPTY = 0,
	TILE_EMPTY_COVERED = -1,
	TILE_MINE_COVERED = -2,
	TILE_EMPTY_FLAG = -3,
	TILE_MINE_FLAG = -4,
	TILE_MINE = -5
};

// globals
string ibuf;
vector<vector<int> > board;
int last_input = 0;
int swidth = 10;
int sheight = 10;
int board_offset_x = 1;
int board_offset_y = 2;
int cursorx = 0;
int cursory = 0;



int main() {
	initscr();
	start_color();
	noecho();
	keypad(stdscr, true);
	
	init_pair(1, COLOR_WHITE, COLOR_BLACK);
	init_pair(2, COLOR_RED, COLOR_BLACK);
	init_pair(3, COLOR_BLUE, COLOR_BLACK);
	
	swidth = LINES;
	sheight = COLS;
	
	make_board();
	redraw();
	int doloop = 1;
	int doredraw = 0;

	while (doloop) {
		move(board_offset_y + cursory, board_offset_x + cursorx*2);
		last_input = getch();
	
		switch (last_input) {
		 case 113:
		 case 27:
		 	doloop = false;
			break;
		 case 258:
		 	if (cursory < board.size()-1)
		 		cursory++;
			break;
		 case 259:
		 	if (cursory > 0)
				cursory--;
			break;
		 case 260:
		 	if (cursorx > 0)
				cursorx--;
			break;
		 case 261:
		 	if (cursorx < board[0].size()-1)
				cursorx++;
			break;
		 case 32:
		 	doredraw = uncover(cursorx, cursory);
		 	break;
		 case 120:
		 	flag(cursorx, cursory);
		 	break;
		}
		
		int r = game_result();
		if (r > 0) {
			setcolor('?');
			move(24, 1);
			if (r == 1)
				printw("you win!");
			else if (r == 2)
				printw("you died!");
			getch();
			break;
		}
		
		if (doredraw)
			redraw();
		doredraw = 0;
		
		setcolor(0);
		move(0, 7);
		printw("%d  ", last_input);
	}

	endwin();
}


int redraw() {
	clear();
	setcolor(0);
	move(0, 0);
	printw(" mine  %d", last_input);
	
	for (int y = 0; y < board.size(); y++) {
		move(board_offset_y + y, board_offset_x);
		for (int x = 0; x < board[0].size(); x++) {
			char c = gettile(board[y][x]);
			setcolor(c);
			printw( "%c ", c );
		}
		printw("\n");
	}
	
	refresh();
	return 0;
}


char gettile(int t) {
	switch (t) {
	 case TILE_EMPTY:
	 	return ' ';
	 case TILE_EMPTY_COVERED:
	 case TILE_MINE_COVERED:
	 	return '#';
	 case TILE_MINE_FLAG:
	 case TILE_EMPTY_FLAG:
	 	return '?';
	 case TILE_MINE:
	 	return 'X';
	 default:
		return '0'+t;
	}
}


void setcolor(char c) {
	if (c == 'X' || c == '?')
		attron(COLOR_PAIR(2));
	else if (c >= '0' && c <= '5')
		attron(COLOR_PAIR(3));
	else
		attron(COLOR_PAIR(1));
}


int make_board() {
	srand(time(NULL));
	board.erase(board.begin(), board.end());
	for (int y = 0; y < 20; y++) {
		board.push_back(vector<int>());
		for (int x = 0; x < 20; x++) {
			int i = rand() % 5;
			int t = ( i >= 1 ? -1 : -2 );
			board[y].push_back(t);
		}
	}
	return 0;
}


int uncover(int xpos, int ypos) {
	if (xpos < 0 || xpos >= board[0].size() || ypos < 0 || ypos >= board.size())
		return 1;
		
	int &t = board[ypos][xpos];
	switch (t) {
	 case TILE_EMPTY_COVERED:
	 case TILE_EMPTY_FLAG:
		t = count_near(xpos, ypos);
		if (t == 0) {
			for (int y = -1; y <= +1; y++)
				for (int x = -1; x <= +1; x++)
					uncover(xpos+x, ypos+y);
			// return 1;
			break;
		}
		break;
	 case TILE_MINE_COVERED:
	 case TILE_MINE_FLAG:
	 	t = TILE_MINE;
	 	break;
	}
	
	move(board_offset_y + ypos, board_offset_x + xpos*2);
	char c = gettile(t);
	setcolor(c);
	printw("%c ", c);
	return 0;
}


int flag(int xpos, int ypos) {
	int &t = board[ypos][xpos];
	switch (t) {
	 case TILE_EMPTY_COVERED:
	 	t = TILE_EMPTY_FLAG;
	 	break;
	 case TILE_EMPTY_FLAG:
	 	t = TILE_EMPTY_COVERED;
	 	break;
	 case TILE_MINE_COVERED:
	 	t = TILE_MINE_FLAG;
	 	break;
	 case TILE_MINE_FLAG:
	 	t = TILE_MINE_COVERED;
	 	break;
	}
	
	move(board_offset_y + ypos, board_offset_x + xpos*2);
	char c = gettile(t);
	setcolor(c);
	printw("%c ", c);
	return 0;
}


int count_near(int xpos, int ypos) {
	int n = 0;
	for (int y = -1; y <= +1; y++) {
		if (ypos+y < 0 || ypos+y >= board.size())
			continue;
		for (int x = -1; x <= +1; x++) {
			if (xpos+x < 0 || xpos+x >= board[0].size())
				continue;
			int t = board[ypos+y][xpos+x];
			if (t == -2 || t == -4 || t == -5)
				n++;
		}
	}
	return n;
}


int mine_count() {
	int n = 0;
	for (auto row : board)
		for (auto t : row)
			if (t == TILE_MINE_COVERED || t == TILE_MINE_FLAG || t == TILE_MINE)
				n++;
	return n;
}
// int unflagged_count() {
// 	int n = 0;
// 	for (auto row : board)
// 		for (auto t : row)
// 			if (t == TILE_MINE_COVERED)
// 				n++;
// 	return n;
// }
int flagged_mine_count() {
	int n = 0;
	for (auto row : board)
		for (auto t : row)
			if (t == TILE_MINE_FLAG)
				n++;
	return n;
}
int flag_count() {
	int n = 0;
	for (auto row : board)
		for (auto t : row)
			if (t == TILE_MINE_FLAG || t == TILE_EMPTY_FLAG)
				n++;
	return n;
}
int mine_exploded() {
	for (auto row : board)
		for (auto t : row)
			if (t == TILE_MINE)
				return 1;
	return 0;
}


int game_result() {
	if (mine_exploded())
		return 2;
	if (flagged_mine_count() == mine_count())
		return 1;
	return 0;
}
