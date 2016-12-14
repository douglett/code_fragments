#include <string>
#include <vector>
#include <algorithm>
#include "xdlib/xdlib.h"
#include "game.h"

using namespace std;


namespace cstr {
	// const int CSTR_MAX = 100;
	char cstr[CSTR_MAX];
	string strtolower(const string& s) {
		string ss = s;
		for (auto& c : ss)
			c = tolower(c);
		return ss;
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