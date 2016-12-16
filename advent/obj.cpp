#include <string>
#include <vector>
#include <algorithm>
#include "xdlib/xdlib.h"
#include "game.h"

using namespace std;


namespace util {
	// const int CSTR_MAX = 100;
	char cstr[CSTR_MAX];
	string strtolower(const string& s) {
		string ss = s;
		for (auto& c : ss)
			c = tolower(c);
		return ss;
	}
	int indexOf(const string& needle, const vector<string>& haystack) {
		for (int i = 0; i < haystack.size(); i++)
			if (haystack[i] == needle)
				return i;
		return -1;
	}
} // end cstr



namespace log {
	static int dirty = 1, dirtyi = 0;
	int screenw = 0, screenh = 0;
	string input;
	vector<string> vlog;
	pair<string, uint32_t> title;

	void log(const string& s) {
		vlog.push_back(s);
		dirty = 1;
	}
	void paint() {
		static const int MTOP = 0, MLEFT = 4, LH = 8;
		if (dirty) {
			auto* dat = xd::screen::backbuffer->getdata();
			xd::draw::clear(dat);
			// title
			xd::draw::fillrect(dat, 0, MTOP, screenw, LH, title.second);
			xd::text::print(dat, title.first, MLEFT, MTOP);
			// main text
			int start = max(0, int(vlog.size()) - LOG_LEN);
			int end   = min(int(vlog.size()), start + LOG_LEN);
			for (int i = 0; i < end - start; i++)
				xd::text::print(dat, vlog[start + i], MLEFT, (MTOP + (i + 1) * LH));
			dirty  = 0;
			dirtyi = 1;  // force repaint input
		}
		if (dirtyi) {
			// input
			auto* dat = xd::screen::backbuffer->getdata();
			int top = MTOP + ((LOG_LEN + 1) * LH);
			xd::draw::fillrect(dat, 0, top, screenw, LH, 0x333333ff);
			xd::text::print(dat, "@"+input, MLEFT, top);
			xd::text::print(dat, "_", (MLEFT + (input.length() + 1) * 8), top);
			dirtyi = 0;
		}
	}
	void inputc(char c) {
		if (input.length() >= LOG_WIDTH-2)  return;
		input += c;
		dirtyi = 1;
	}
	void delc() {
		input.pop_back();
		dirtyi = 1;
	}
	void clearc() {
		input = "";
		dirtyi = 1;
	}
} // end log



namespace map {
	static int id = 0;
	int posx = 0, posy = 0;
	vector<string> umap, map;

	int isexit(char c) {
		switch (c) {
		case 'n':  if (posy > 0                  && map[posy-1][posx] != ' ')  return 1;  break;
		case 's':  if (posy < map.size()-1       && map[posy+1][posx] != ' ')  return 1;  break;
		case 'w':  if (posx > 0                  && map[posy][posx-1] != ' ')  return 1;  break;
		case 'e':  if (posx < map[posy].size()-1 && map[posy][posx+1] != ' ')  return 1;  break;
		case 'd':  if (map[posy][posx] == '/')  return 1;  break;
		}
		return 0;
	}
	int movepos(char c) {
		int mv = 0;
		switch (c) {
		case 'n':  if (isexit('n'))  posy--, mv = 1;  break;
		case 's':  if (isexit('s'))  posy++, mv = 1;  break;
		case 'e':  if (isexit('e'))  posx++, mv = 1;  break;
		case 'w':  if (isexit('w'))  posx--, mv = 1;  break;
		case 'd':  if (isexit('d'))  lprintf("you go down");  nextmap();  mv = 1;  break;
		}
		umap[posy][posx] = map[posy][posx];
		return mv;
	}

	void showmap() {
		lprintf("+%s+", string(map::umap[0].size(), '-').c_str());  // top
		for (int y = 0; y < map::umap.size(); y++) {
			string s = map::umap[y];
			if (y == map::posy)  s[map::posx] = '@';
			lprintf("|%s|", s.c_str());
		}
		lprintf("+%s+", string(map::umap.back().size(), '-').c_str());  // bottom
	}

	void nextmap() {
		id++;
		switch (id) {
		case 1:
			map = {
				"...  ",
				"  .  ",
				" ..x ",
				" x . ",
				"   . ",
				"./.. "
			};
			posx = posy = 0;
			break;
		case 2:
			map = {
				" . ",
				"..x",
				" . "
			};
			posx = 1, posy = 0;
			break;
		} // end switch
		// create user map
		umap = {};
		for (const auto& s : map)
			umap.push_back( string(s.length(), ' ') );
		movepos(0);  // fill first position
	}

	char current() {
		return map[posy][posx];
	}

	string roomdesc() {
		switch (current()) {
		case '.':  return "dark room";
		case 'x':  return "webbed room";
		case '/':  return "staircase";
		default:   return "??";
		} // end switch
	}

	string exitstr() {
		string e, dir = "nsewd";
		for (auto c : dir)
			if (isexit(c))  e += c;
		return e;
	}
} // end map