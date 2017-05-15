#include "globals.h"

using namespace std;


namespace vram {
	SDL_Surface *vmem=NULL;
	vector<array<int,6>> sprlist;

	void init() {
		if (!SDL_WasInit(0)) {
			fprintf(stderr, "error: tried to init vram before SDL was initialized.\n");
			exit(1);
		}
		SDL_FreeSurface(vmem);
		vmem = vid::makesurface(640, 480);  // setup background video memory
	}

	int is_init() {
		return ( vmem != NULL );
	}

	int flip() {
		SDL_Rect  rs={0,0,320,240},  rd;
		SDL_BlitSurface(vmem, &rs, vid::screen, &rs);
		for (auto& spr : sprlist) {
			rs.x=spr[0],  rs.y=spr[1],  rs.w=spr[2],  rs.h=spr[3];
			rd.x=spr[4],  rd.y=spr[5];
			SDL_BlitSurface(vmem, &rs, vid::screen, &rd);
		}
		vid::scaleto(vid::screen, vid::screen);  // scale up in place before flipping
		return 0;
	}

	int dbgflip() {
		SDL_BlitSurface(vmem, NULL, vid::screen, NULL);
		return 0;
	}

	void testscene() {
		SDL_Rect r, r2;
		// draw main background
		r={0,0,320,240};
		SDL_FillRect(vmem, &r, SDL_MapRGB(vmem->format, 255,0,0));
		// draw sprite in vmem
		r={320,0,40,40};
		SDL_FillRect(vmem, &r, SDL_MapRGB(vmem->format, 255,255,0));
		r={320,0,1,1};
		for (int i=0; i<40; i++) {
			SDL_FillRect(vmem, &r, SDL_MapRGB(vmem->format, 0,0,255));
			r.x++;  r.y++;
		}
		r={330,10,20,20};
		SDL_FillRect(vmem, &r, SDL_MapRGB(vmem->format, 255,0,255));
		// r={10,400,20,20};
		// SDL_FillRect(vmem, &r, SDL_MapRGB(vmem->format, 255,0,255));
		
		r ={0,400,40,40};
		r2={10,250,40,40};
		SDL_BlitSurface(vmem, &r, vmem, &r2);

		// create sprite
		sprlist.push_back({{ 320,0,40,40, 10,10 }});
		sprlist.push_back({{ 10,250,40,40, 100,40 }});

		// vid::video_mode=3;
	}
} // end vram