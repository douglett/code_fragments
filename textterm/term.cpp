#include "globals.h"
#include <iostream>

using namespace std;


namespace term {
	// public vars
	char c_str[C_MAX]={0};
	vector<string> texthist;

	void log(const string& s) {
		texthist.push_back(s);
		printf("%s\n", s.c_str());
	}

	int flip() {
		SDL_FillRect(vid::screen, NULL, SDL_MapRGB(vid::screen->format, 0,0,0));  // cls
		char c;
		SDL_Rect rd={0};
		for (int i=0; i<term::texthist.size(); i++)
		for (int j=0; j<term::texthist[i].length(); j++) {
			c  = term::texthist[i][j];
			rd.x=j*8+8,     rd.y=i*8+8;     // dest on screen
			SDL_BlitSurface(vid::textc(c), NULL, vid::screen, &rd);
		}
		// vid::scaleto(screen, screen);  // scale up in place before flipping
		return 0;
	}
} // end term