#pragma once
#include <string>
#include <ncurses.h>

namespace curs {
	int  init();
	int  quit();
	int  pause();
	int  width();
	int  height();
	// void move(int x, int y);
	// void prints(const std::string& s);
} // end curs