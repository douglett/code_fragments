#include <iostream>
#include <algorithm>
#include "xdlib/xdlib.h"

using namespace std;

const int CSTR_MAX = 100;
char cstr[CSTR_MAX];
int running = 1;


namespace log {
	const int LOG_LEN = 19;
	int dirty = 1;
	vector<string> vlog;

	void log(const string& s) {
		vlog.push_back(s);
		dirty = 1;
	}
	void paint() {
		auto* dat = xd::screen::backbuffer->getdata();
		xd::draw::clear(dat);
		int start = max(0, int(vlog.size()) - LOG_LEN);
		int end   = min(int(vlog.size()), start + LOG_LEN);
		for (int i = 0; i < end - start; i++)
			xd::text::prints(dat, vlog[start + i], 4, 4+i*8);
		dirty = 0;
	}
} // end log


static void keycb(int key, int status) {
	static int count = 0;
	if (!status)  return;
	switch (key) {
	case 27:   running = 0;  break;
	case ' ':  snprintf(cstr, CSTR_MAX, "hello world %d", count++); log::log(cstr);  break;
	}
}


int main() {
	cout << "hello" << endl;
	xd::screen::init();
	xd::screen::keycb = keycb;

	log::log("hello");
	log::log("world");
	log::log("lol");

	while (running) {
		if (log::dirty)  log::paint();
		if (xd::screen::paint())  break;
	}

	xd::screen::quit();
}