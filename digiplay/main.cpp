#include "graphics/graphics.h"
#include "helpers/helpers.h"
#include <iostream>
#include <vector>

using namespace std;
using namespace helpers;


namespace screentxt {
	void log(const string& s);
	// 
	const int maxw = 39, maxh = 29, offx = 4, offy = 4;
	int dopaint = 1;
	vector<string> lines;
	string inputstr;

	int handlekey(int key, int val) {
		// printf("key: %d %d\n", key, val);
		if      (val == 0) { }
		else if (key == SDLK_ESCAPE) { return 1; }
		else if (key == SDLK_BACKSPACE) { if (inputstr.size()) inputstr.pop_back();  dopaint = 1; }
		else if (key == SDLK_RETURN) {
			string s = lines.back() + inputstr;
			inputstr = "";
			lines.pop_back();
			log(s), log("");
		}
		else if (key >= ' ' && key <= '~') { inputstr += char(key);  dopaint = 1; }
		return 0;
	}
	
	void log(const string& s) {
		vector<string> vs = { s };
		while (vs.back().length() > maxw) {
			string s = vs.back().substr(maxw);
			vs.back() = vs.back().substr(0, maxw);
			vs.push_back(s);
		}
		lines.insert(lines.end(), vs.begin(), vs.end());
		if (lines.size() > maxh)
			lines.erase(lines.begin(), lines.begin() + (lines.size() - maxh));
		dopaint = 1;
	}

	void paint(SDL_Surface* buf) {
		if (!dopaint)  return;
		SDL_FillRect(buf, NULL, gfx::drawc(0,0,0));
		gfx::drawc(255,255,255);
		for (int i=0; i<lines.size()-1; i++)
			gfx::drawstr(buf, offx, offy + i*8, lines[i]);
		gfx::drawstr(buf, offx, offy + (maxh-1)*8, lines.back() + inputstr + char(2));
		dopaint = 0;
	}
} // end screentxt


int main(int argc, char** argv) {
	gfx::init(640, 480, "blah");
	gfx::handlekey = screentxt::handlekey;
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
	for (int i=0; i<=24; i++)
		screentxt::log(strfmt("%d", i));
	
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