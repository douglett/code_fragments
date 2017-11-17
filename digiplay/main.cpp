#include "graphics/graphics.h"
#include <iostream>
#include <vector>

using namespace std;


namespace screentxt {
	const int maxw = 39, maxh = 29, offx = 4, offy = 4;
	int dopaint = 1;
	vector<string> lines;
	
	void log(const string& s) {
		vector<string> vs = { s };
		while (vs.back().length() > maxw) {
			string s = vs.back().substr(maxw);
			vs.back() = vs.back().substr(0, maxw);
			vs.push_back(s);
		}
		lines.insert(lines.end(), vs.begin(), vs.end());
		if (lines.size() > maxw)
			lines.erase(lines.begin(), lines.begin() + (lines.size() - maxw));
		dopaint = 1;
	}

	void paint(SDL_Surface* buf) {
		if (!dopaint)  return;
		SDL_FillRect(buf, NULL, gfx::drawc(0,0,0));
		gfx::drawc(255,255,255);
		for (int i=0; i<lines.size(); i++)
			gfx::drawstr(buf, offx, offy + i*8, lines[i]);
		dopaint = 0;
	}
} // end screentxt


int main(int argc, char** argv) {
	gfx::init(640, 480, "blah");
	SDL_Surface* screen = SDL_GetVideoSurface();
	SDL_Surface* buftxt = gfx::mksprite(320, 240);
	SDL_Surface* bufgfx = gfx::mksprite(320, 240);
	// init buffers
	SDL_Rect scrrect = { 0, 0, 320, 240 };
	SDL_FillRect(bufgfx, &scrrect, gfx::drawc(255,0,255));
	// gfx::drawc(255,255,255);
	// for (int i=0; i<29; i++)
	// 	gfx::drawstr(buftxt, 4, 4+i*8, string(39, 'X'));
	screentxt::log("hello world!");
	screentxt::log("I am doug.");
	screentxt::log("bacon bacon bacon bacon bacon bacon bacon bacon bacon bacon bacon bacon bacon bacon bacon bacon bacon bacon ");
	
	while (true) {
		// SDL_Rect r = {0};
		screentxt::paint(buftxt);
		SDL_BlitSurface(buftxt, &scrrect, screen, &scrrect);
		SDL_BlitSurface(bufgfx, &scrrect, screen, &scrrect);
		gfx::scale2x(screen, screen);
		if (gfx::flip())  break;
	}
	
	SDL_Quit();
	return 0;
}