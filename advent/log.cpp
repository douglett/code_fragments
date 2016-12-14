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
	int dirty = 1, dirtyi = 0;
	int screenw = 0, screenh = 0;
	string input;
	vector<string> vlog;

	void log(const string& s) {
		vlog.push_back(s);
		dirty = 1;
	}
	void paint() {
		if (dirty) {
			auto* dat = xd::screen::backbuffer->getdata();
			xd::draw::clear(dat);
			int start = max(0, int(vlog.size()) - LOG_LEN);
			int end   = min(int(vlog.size()), start + LOG_LEN);
			for (int i = 0; i < end - start; i++)
				xd::text::print(dat, vlog[start + i], 4, 2+i*8);
			dirty  = 0;
			dirtyi = 1;  // force repaint input
		}
		if (dirtyi) {
			auto* dat = xd::screen::backbuffer->getdata();
			int top = 2 + LOG_LEN*8 + 3;
			xd::draw::fillrect(dat, 0, top-1, screenw, 10, 0x333333ff);
			xd::text::print(dat, "@"+input, 4, top);
			xd::text::print(dat, "_", 4 + (input.length()+1)*8, top);
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
} // end log