#include "graphics/graphics.h"
#include <vector>
#include <list>
#include <sstream>
#include <algorithm>

using namespace std;


std::list<std::string>& PIPE_1() {
	static list<string> ls={
		// "locate 1 1",
		"printf \"hello world\"",
		"locate 0 22",
		"printf \"test 2\"",
		"getch"
	};
	return ls;
}
void PIPE_2(std::list<std::string>& ls) {
}


int getchk=0;
int termw=39, termh=22, termx=0, termy=0;
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

string escapestr(string str) {
	if (str.size() && str[0]=='"')  str=str.substr(1);
	if (str.size() && str.back()=='"')  str.pop_back();
	return str;
}

int handlecmd(const string& cmdstr) {
	vector<string> cmd=split(cmdstr);
	// handle command
	// if (cmd.size()==0 || cmd[0]=="" || cmd[0]=="error") {
	// 	return 0;
	// }
	if (cmd[0]=="locate" && cmd.size()==3) {
		termx = max(min((int)stol(cmd[1]), termw), 0);
		termy = max(min((int)stol(cmd[2]), termh), 0);
		printf("locate: %d %d\n", termx, termy);
		return 1;
	}
	else if (cmd[0]=="printf" && cmd.size()>=2) {
		gfx::drawstr(scr, 4+termx*8, 4+termy*10, escapestr(cmd[1]));
		termy+=1, termx=0;
		return 1;
	}
	else if (cmd[0]=="getch") {
		if (getchk<1)  return 0;
		response.push_back("getch "+to_string(getchk));
		printf("getch: %d\n", getchk);
		return 1;
	}
	else if (cmd[0]=="getline") {

	}
	// unknown command
	fprintf(stderr, "error: %s\n", cmd[0].c_str());
	return -1;
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
		while (PIPE_1().size()) {
			int p = handlecmd(PIPE_1().front());
			if      (p==0)   { break; }
			else if (p==1)   { PIPE_1().pop_front(); }
			else if (p==-1)  { err=1;  break; }
		}
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