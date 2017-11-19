#include "graphics/graphics.h"
#include "helpers/helpers.h"
#include <iostream>
#include <vector>

using namespace std;
using namespace helpers;


SDL_Surface *screen, *buftxt, *bufgfx; 


namespace screentxt {
	int parseline(const string& ln);
	void log(const string& s);
	void prompt();
	// 
	const int maxw = 39, maxh = 29, offx = 4, offy = 4;
	int view = 2, dopaint = 1;
	vector<string> lines;
	string inputstr;

	int handlekey(int key, int val) {
		// printf("key: %d %d\n", key, val);
		if      (val == 0) { }
		else if (key == SDLK_ESCAPE) { return 1; }
		else if (key == SDLK_F1) { view = 1; }
		else if (key == SDLK_F2) { view = 2; }
		else if (key == SDLK_BACKSPACE) { if (inputstr.size()) inputstr.pop_back();  dopaint = 1; }
		else if (key == SDLK_RETURN) {
			string s = lines.back() + inputstr;
			lines.pop_back();
			log(s);
			parseline(inputstr);
			prompt();
		}
		else if (key >= ' ' && key <= '~') { inputstr += char(key);  dopaint = 1; }
		// else    { printf("key: %d\n", key); }
		return 0;
	}

	int parseline(const string& ln) {
		auto vs = splitws(ln);
		if (vs.size()==0) { return 0; }
		// printf("here\n");
		if ((vs[0]=="line" || vs[0]=="box" || vs[0]=="boxf") && vs.size() == 5) {
			int x1 = stoi(vs[1], 0, 0);
			int y1 = stoi(vs[2], 0, 0);
			int x2 = stoi(vs[3], 0, 0);
			int y2 = stoi(vs[4], 0, 0);
			// printf("here 2  %d %d %d %d\n", x1, y1, x2, y2);
			if      (vs[0]=="line") { gfx::drawline(bufgfx, x1, y1, x2, y2); }
			else if (vs[0]=="box" ) { gfx::drawbox(bufgfx, x1, y1, x2, y2); }
			else if (vs[0]=="boxf") { gfx::drawboxf(bufgfx, x1, y1, x2, y2); }
			return 0;
		}
		else if (vs[0]=="color" && vs.size() == 2) {
			if      (vs[1] == "white"  )  gfx::drawc(255,255,255);
			else if (vs[1] == "black"  )  gfx::drawc(0,0,0);
			else if (vs[1] == "red"    )  gfx::drawc(255,0,0);
			else if (vs[1] == "green"  )  gfx::drawc(0,255,0);
			else if (vs[1] == "blue"   )  gfx::drawc(0,0,255);
			else if (vs[1] == "yellow" )  gfx::drawc(255,255,0);
			else if (vs[1] == "purple" )  gfx::drawc(255,0,255);
			else if (vs[1] == "cyan"   )  gfx::drawc(0,255,255);
			else    { log("unknown color");  return 1; }
			return 0;
		}
		else if (vs[0]=="colorcode" && vs.size() == 4) {
			int r = stoi(vs[1], 0, 0);
			int g = stoi(vs[2], 0, 0);
			int b = stoi(vs[3], 0, 0);
			gfx::drawc(r, g, b);
			return 0;
		}
		// unknown
		log("unknown command");
		return 1;
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

	void prompt() {
		log("> ");
		inputstr = "";
		dopaint = 1;
	}

	void repaint() {
		if (!dopaint)  return;
		auto col = gfx::drawc();
		SDL_FillRect(buftxt, NULL, gfx::drawc(0,0,0));
		// gfx::drawc(255,255,255);
		for (int i=0; i<lines.size()-1; i++)
			gfx::drawstr(buftxt, offx, offy + i*8, lines[i]);
		gfx::drawstr(buftxt, offx, offy + (maxh-1)*8, lines.back() + inputstr + char(2));
		gfx::drawc(col);
		dopaint = 0;
	}
} // end screentxt


int main(int argc, char** argv) {
	gfx::init(640, 480, "blah");
	gfx::handlekey = screentxt::handlekey;
	screen = SDL_GetVideoSurface();
	buftxt = gfx::mksprite(320, 240);
	bufgfx = gfx::mksprite(320, 240);
	// init buffers
	SDL_Rect scrrect = { 0, 0, 320, 240 };
	SDL_FillRect(bufgfx, &scrrect, gfx::drawc(255,0,255));
	gfx::drawc(255,255,255);
	// for (int i=0; i<29; i++)
	// 	gfx::drawstr(buftxt, 4, 4+i*8, string(39, 'X'));
	screentxt::log("hello world!");
	screentxt::log("I am doug.");
	screentxt::log("bacon bacon bacon bacon bacon bacon bacon bacon bacon bacon bacon bacon bacon bacon bacon bacon bacon bacon ");
	for (int i=0; i<=24; i++)
		screentxt::log(strfmt("%d", i));
	screentxt::prompt();
	
	while (true) {
		// SDL_Rect r = {0};
		printf("color: %x\n", gfx::drawc());
		screentxt::repaint();
		SDL_BlitSurface(buftxt, &scrrect, screen, &scrrect);
		if (screentxt::view == 2)  SDL_BlitSurface(bufgfx, &scrrect, screen, &scrrect);
		gfx::scale2x(screen, screen);
		if (gfx::flip())  break;
	}
	
	SDL_Quit();
	return 0;
}