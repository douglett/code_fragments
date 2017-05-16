#include "globals.h"
#include <iostream>

using namespace std;


namespace term {
	// public vars
	char c_str[C_MAX]={0};
	std::vector<std::pair<int, std::string>> texthist;

	void log(const std::string& s) {
		texthist.push_back({ 0, s });
		printf("%s\n", s.c_str());
	}
	void err(const std::string& s) {
		texthist.push_back({ 1, s });
		fprintf(stderr, "%s\n", s.c_str());
	}

	int flip() {
		SDL_FillRect(vid::screen, NULL, SDL_MapRGB(vid::screen->format, 0,0,0));  // cls
		char c;
		SDL_Rect rd={0};
		uint32_t col;
		for (int i=0; i<texthist.size(); i++)
		for (int j=0; j<texthist[i].second.length(); j++) {
			c   = texthist[i].second[j];
			col = ( texthist[i].first==0 ? vid::fgcolor : vid::errcolor );
			rd.x=j*8+8,     rd.y=i*10+8;     // dest on screen
			SDL_BlitSurface(vid::textc(c, col), NULL, vid::screen, &rd);
		}
		// vid::scaleto(screen, screen);  // scale up in place before flipping
		return 0;
	}
} // end term