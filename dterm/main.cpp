#include "graphics/graphics.h"
#include <vector>
#include <list>
#include <sstream>
#include <algorithm>

using namespace std;


std::list<std::string>& PIPE_1() {
	static list<string> ls={
		"locate 1 1",
		"printf \"hello world\\n\"",
		"getch"
	};
	return ls;
}
void PIPE_2(std::list<std::string>& ls) {
}


vector<string> split(const string& ln) {
	vector<string> vs={""};
	for (int i=0; i<ln.length(); i++) {
		char c=ln[i];
		if      (isspace(c)) { if (vs.back().length()) vs.push_back(""); }
		else if (c=='"')
			for (vs.back()+='"', i++; i<ln.size(); i++) {
				vs.back()+=ln[i];
				if (ln[i]=='"') break;
			}
		else    vs.back()+=c;
	}
	if (vs.size()==1 && vs.back().length()==0) vs.pop_back();
	return vs;
}


list<string> response;


int getch() {
	int rval=0;
	SDL_Event e;
	while (SDL_PollEvent(&e)) {
		if (e.type==SDL_QUIT) rval=-1;
		if (e.type==SDL_KEYDOWN) 
			switch (e.key.keysym.sym) {
			case 27:  rval=-1;  break;
			default:  if (!rval) rval=e.key.keysym.sym;
			}
	}
	return rval;
}


int main(int argc, char** argv) {
	printf("start\n");

	gfx::init(640, 480, "win");
	gfx::handleEvents=0;
	SDL_Surface* scr = gfx::mksprite(320, 240);
	SDL_FillRect(scr, NULL, 0x000000ff);

	int termw=39, termh=29, termx=0, termy=0;

	int err=0, getchk=0, shown=1;
	while (err==0 && getchk!=-1) {
		auto& inp = PIPE_1();  // get input from the master pipe
		while (inp.size()) {
			auto vs=split( inp.front() );
			if (!shown) {
				for (auto& s : vs) printf("  [%s]\n", s.c_str());
				shown=1;
			}
			if      (vs.size()==0)  continue;
			else if (vs[0]=="locate" && vs.size()==3) {
				termx = max(min((int)stol(vs[1]), termw), 0);
				termy = max(min((int)stol(vs[2]), termh), 0);
				printf("locate: %d %d\n", termx, termy);
			}
			else if (vs[0]=="printf" && vs.size()>=2) {
				gfx::drawstr(scr, 4+termx*8, 4+termy*10, vs[1]);
			}
			else if (vs[0]=="getch") {
				if (!getchk) break;
				printf("getch: %d\n", getchk);
				response.push_back("getch "+to_string(getchk));
			}
			else {
				fprintf(stderr, "error: %s\n", inp.front().c_str());
				err=1;  break;
			}
			inp.pop_front();
			shown=0;
		}
		// paint
		SDL_FillRect(SDL_GetVideoSurface(), NULL, 0x000000ff);
		gfx::scale2x(scr, SDL_GetVideoSurface());
		getchk=getch();
		gfx::flip();
	}

	SDL_Quit();
	return err;
}