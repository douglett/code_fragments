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


int getchk=0;
int termw=39, termh=29, termx=0, termy=0;
list<string> response;
SDL_Surface* scr=NULL;


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

vector<string> nextcmd() {
	vector<string> vs;
	if (PIPE_1().size()==0) return vs;
	vs=split( PIPE_1().front() );
	vs.insert(vs.begin(), PIPE_1().front());
	PIPE_1().pop_front();
	return vs;
}

int handlecmd() {
	static vector<string> cmd;
	while (true) {
		// get command from buffer or pipe
		if (cmd.size()<2) cmd=nextcmd();
		if (cmd.size()<2) break;
		// handle command
		if (cmd[1]=="error") break;
		else if (cmd[1]=="locate" && cmd.size()==4) {
			termx = max(min((int)stol(cmd[2]), termw), 0);
			termy = max(min((int)stol(cmd[3]), termh), 0);
			printf("locate: %d %d\n", termx, termy);
		}
		else if (cmd[1]=="printf" && cmd.size()>=3) {
			gfx::drawstr(scr, 4+termx*8, 4+termy*10, cmd[2]);
		}
		else if (cmd[1]=="getch") {
			if (getchk<1) break;
			response.push_back("getch "+to_string(getchk));
			printf("getch: %d\n", getchk);
		}
		else {
			fprintf(stderr, "error: %s\n", cmd[0].c_str());
			cmd={"error", "error"};
			break;
		}
		cmd={};
	}
	return 0;
}


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
	scr = gfx::mksprite(320, 240);
	SDL_FillRect(scr, NULL, 0x000000ff);

	int err=0;
	while (err==0) {
		// handle input commands
		handlecmd();
		// send any pending commands
		PIPE_2(response);
		// paint
		SDL_FillRect(SDL_GetVideoSurface(), NULL, 0x000000ff);
		gfx::scale2x(scr, SDL_GetVideoSurface());
		getchk=getch();
		if (getchk==-1) err=1;
		gfx::flip();
	}

	SDL_Quit();
	return err;
}