#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include "game.h"

using namespace std;


namespace loc {
	
	static vector<string> map = {
		"..... ",
		".. . ",
		".....",
		" . . ",
		" ... "
	};
	
	static vector<shared_ptr<Thingum>> thingums;
	
	static int  xpos = 0,  ypos = 0;
	
	void setup_thingums() {
		thingums.push_back(make_shared<Wizard>());
	}

	shared_ptr<Thingum> get_thingum_at(int x, int y) {
		for (auto t : thingums) {
			if (t->xpos == x && t->ypos == y) {
				return t;
			}
		}
		return NULL;
	}

	void show_thingums() {
		shared_ptr<Thingum> t = get_thingum_at(xpos, ypos);
		if (t == NULL) {
			return;
		}

		scr::println(t->name + " is here.");

	}
	
	
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
		if (c == 'n'  && ypos > 0 && map[ypos-1][xpos] != ' ')  return 1;
		if (c == 's'  && ypos < map.size()-1 && map[ypos+1][xpos] != ' ')  return 1;
		if (c == 'w'  && xpos > 0 && map[ypos][xpos-1] != ' ')  return 1;
		if (c == 'e'  && xpos < map[ypos].size()-1 && map[ypos][xpos+1] != ' ')  return 1;
		return 0;
	}

	int move(char c) {
		if (!test_exit(c))  return 0;
		if (c == 'n')  { ypos--;  return 1; }
		if (c == 's')  { ypos++;  return 1; }
		if (c == 'w')  { xpos--;  return 1; }
		if (c == 'e')  { xpos++;  return 1; }
		return 0;
	}

	string submap(int w, int h) {
		int  ys = ypos - h/2,  xs = xpos - w/2;
		string s;
		for (int y = ys; y < ys+h; y++)
			for (int x = xs; x < xs+w; x++)
				if (x < 0 || y < 0 || x >= map[ypos].size() || y >= map.size())  s += ' ';
				else  s += map[y][x];
		return s;
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
