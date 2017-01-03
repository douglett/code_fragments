#pragma once

#include <vector>
#include <string>

namespace gmap {
	extern int  posx, posy, dir;
	extern std::vector<std::string> gmap;
	int is_empty(char c);
	int is_null(char c);
	int is_nothing(char c);
	std::string getrow(int row);
} // end gmap