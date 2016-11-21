#include <iostream>
#include <vector>
#include <string>
#include "game.h"

using namespace std;


namespace loc {
	
	static vector<string> map = {
		"..   ",
		"..   ",
		".....",
		" .   ",
		" .   "
	};
	static int  x = 0,  y = 0;

	static string join(const vector<string>& vs, const string& glue = ", ") {
		int started = 0;
		string str;
		for (auto& s : vs) {
			str += ( started ? glue : "" ) + s;
			started = 1;
		}
		return str;
	}

	int test_exit(char c) {
		if (c == 'n'  && y > 0 && map[y-1][x] != ' ')  return 1;
		if (c == 's'  && y < map.size()-1 && map[y+1][x] != ' ')  return 1;
		if (c == 'w'  && x > 0 && map[y][x-1] != ' ')  return 1;
		if (c == 'e'  && x < map[0].size()-1 && map[y][x+1] != ' ')  return 1;
		return 0;
	}

	int move(char c) {
		if (!test_exit(c))  return 0;
		if (c == 'n')  { y--;  return 1; }
		if (c == 's')  { y++;  return 1; }
		if (c == 'w')  { x--;  return 1; }
		if (c == 'e')  { x++;  return 1; }
		return 0;
	}

	string exits() {
		vector<string> e;
		if (test_exit('n'))  e.push_back("north");
		if (test_exit('s'))  e.push_back("south");
		if (test_exit('e'))  e.push_back("east");
		if (test_exit('w'))  e.push_back("west");
		return join(e);
	}

} // end loc



int main() {
	cout << "hello world" << endl;
	scr::init();

	while (true) {
		if (scr::get_input())  break;
	}

	scr::quit();
}
