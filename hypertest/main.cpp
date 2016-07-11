#include <iostream>
#include "xdlib/xdlib.h"

using namespace std;


int esc = 0;
void keycb(int key, int stat) {
	if (stat == 0)  return;
	printf("key: %0x \n", key);
	if (key == 27)  esc = 1;
}

void mousecb(int x, int y, int btn) {
	printf("mouse: %d %d %d \n", x, y, btn);
}

void playcb(int x, int y, int btn) {
	if (btn != 1)  return;
	printf("playbtn\n");
}
void stepcb(int x, int y, int btn) {
	if (btn != 1)  return;
	printf("stepbtn\n");
}


xd::screen::Sprite* makebtn(const string& s, int x, int y) {
	auto playbtn = xd::screen::makesprite(s.length()*8+4, 8+4);
	playbtn->pos(x, y);
	auto d = playbtn->getdata();
	// auto bg = xd::draw::rgb(0, 0, 0);
	auto fg = xd::draw::rgb(255, 0, 0);
	// xd::draw::fillrect(d, bg, 0, 0, d[0], d[1]);
	xd::draw::tracerect(d, fg, 0, 0, d[0], d[1]);
	xd::text::prints(d, s, 2, 2, fg);
	return playbtn;
}

const char* num2(uint8_t num) {
	static char s[20];
	snprintf(s, 20, "%02x", num);
	return s;
}
const char* hexnum(uint8_t num, int size = 4) {
	static char s[20];
	snprintf(s, 20, (size == 2 ? "%02x" : "%04x"), num);
	return s;
}


int main(int argc, char** argv) {
	printf("hello world\n");

	xd::screen::init(320, 240, 2);
	int screenw, screenh;
	xd::screen::getinfo(&screenw, &screenh, NULL);
	// set up callbacks
	xd::screen::keycb = keycb;
	// xd::screen::backbuffer->mousecb = mousecb;

	// hello world on green transparent bg
	string s = "hello world!";
	auto spr = xd::screen::makesprite(8*s.length()+2, 8+2);
	{
		spr->pos(0, 0);
		auto d = spr->getdata();
		auto bg = xd::draw::rgba(0, 255, 0, 100);
		auto fg = xd::draw::rgb(255, 0, 0);
		xd::draw::fillrect(d, bg, 0, 0, 8*s.length()+2, 8+2);
		xd::text::prints(d, s, 1, 1, fg);
	}

	// example background image
	auto dscreen = xd::screen::makesprite(128+8, 96+8);
	{
		dscreen->pos(0, 10+4);
		auto dat = dscreen->getdata();
		auto bg = xd::draw::rgb(40, 40, 200);
		xd::draw::fillrect(dat, bg, 0, 0, 128+8, 96+8);
		for (int y = 0; y < 12; y++)
			for (int x = 0; x < 16; x++)
				xd::text::prints(dat, "x", x*8+4, y*8+4);
	}

	// play button
	auto playbtn = makebtn("play", screenw-(8*4+4), 0);
	playbtn->mousecb = playcb;
	// step button
	auto stepbtn = makebtn("step", playbtn->x-(8*4+4+2), playbtn->y);
	stepbtn->mousecb = stepcb;

	// registers box
	auto reg2 = xd::screen::makesprite(8*6+4, 10*12+4);
	{
		auto dat = reg2->getdata();
		auto red = xd::draw::rgb(255, 0, 0);
		auto green = xd::draw::rgb(0, 200, 0);
		reg2->pos(screenw-dat[0], 10+4);
		xd::draw::tracerect(dat, red, 0, 0, dat[0], dat[1]);
		vector<pair<string, int> > labels = {
			{ "a", 00 },
			{ "b", 00 },
			{ "c", 00 },
			{ "x", 00 },
			{ "y", 00 },
			{ "z", 00 },
			{ "i", 00 },
			{ "j", 00 },
			{ "pc", 00 },
			{ "sp", 00 },
			{ "ex", 00 },
			{ "ai", 00 }
		};
		for (int i = 0; i < labels.size(); i++) {
			xd::text::prints(dat, labels[i].first, 2, i*10+2, green);
			xd::text::prints(dat, hexnum(labels[i].second), 8*2+2, i*10+2, red);
		}
	}

	// mainloop
	while (!esc) {
		if (xd::screen::paint())
			esc = 1;
	}

	xd::screen::quit();
	return 0;
}