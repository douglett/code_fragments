#include "globals.h"
#include <iostream>
#include <vector>
#include <SDL.h>

using namespace std;


namespace vid {
	// const int SURFACE_MAX=1024;
	SDL_Surface* screen=NULL;
	Uint32 tcolor=0;
	vector<int> keylist;

	void init() {
		// init library
		if (SDL_Init(SDL_INIT_VIDEO)) {
			fprintf(stderr, "error initialising SDL\n");
			exit(1);
		}
		// create screen
		screen = SDL_SetVideoMode( 640, 480, 32, SDL_SWSURFACE );
		if (screen==NULL) {
			fprintf(stderr, "error making screen surface\n");
			exit(1);
		}
		// first fill
		int err = SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0,0,0));
		if (err) {
			fprintf(stderr, "error making screen surface: first fill failed (%d)\n", err);
			exit(1);
		}
		// set screen properties
		SDL_WM_SetCaption("mywin1", "mywin2");  // window title
		tcolor = SDL_MapRGB(screen->format, 255, 0, 255);  // set transparent color
	}

	SDL_Surface* makesurface(int w, int h) {
		// check bounds
		if (w<0 || w>SURFACE_MAX || h<0 || h>SURFACE_MAX) {
			fprintf(stderr, "error: new surface dimensions of bounds: %d %d\n", w, h);
			exit(1);
		}
		// make surface
		SDL_Surface* box = SDL_CreateRGBSurface(SDL_SWSURFACE, w, h, 32,
			screen->format->Rmask, screen->format->Gmask, screen->format->Bmask, screen->format->Amask);
		if (box==NULL) {
			fprintf(stderr, "error making surface: could not allocate\n");
			exit(1);
		}
		// first fill
		int err = SDL_FillRect(box, NULL, SDL_MapRGB(box->format, 0,0,0));
		if (err) {
			fprintf(stderr, "error making surface: first fill failed (%d)\n", err);
			exit(1);
		}
		// set surface properties
		SDL_SetColorKey(box, SDL_SRCCOLORKEY, tcolor);  // transparency
		return box;  // return
	}

	void scaleto(SDL_Surface* src, SDL_Surface* dst) {
		Uint32* dpx = (Uint32*)dst->pixels;
		Uint32* spx = (Uint32*)src->pixels;
		for (int h=min(src->h,dst->h/2)-1; h>=0; h--)  // go in reverse, in case src=dst
		for (int w=min(src->w,dst->w/2)-1; w>=0; w--) {
			// ((Uint32*)dst->pixels)[h*dst->w + w] = ((Uint32*)src->pixels)[h*src->w + w];
			dpx[h*2*dst->w + (w*2)]     = dpx[h*2*dst->w + (w*2)+1]     = 
			dpx[(h*2+1)*dst->w + (w*2)] = dpx[(h*2+1)*dst->w + (w*2)+1] =
			spx[h*src->w + w];
		}
	}

	int getkeys() {
		keylist={ };
		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			if (e.type==SDL_QUIT)  return -1;
			else if (e.type==SDL_KEYDOWN)
				switch (e.key.keysym.sym) {
				case SDLK_ESCAPE:  return -1;
				default:  keylist.push_back(e.key.keysym.sym);  break;
				}
		}
		return 0;
	}
} // end vid


int main(int argc, char** argv) {
	printf("hello world\n");
	vid::init();
	SDL_Surface* screen=vid::screen;
	// box test
	SDL_Surface* box = vid::makesurface(100, 100);
	SDL_Rect r={30,30,40,40};
	SDL_FillRect(box, &r, vid::tcolor);
	// scale test
	// SDL_Surface* box2 = vid::makesurface(200, 200);
	// vid::scaleto(box, box2);
	// vid::scaleto(box, box);
	
	int loop=1;
	while (loop) {
		if (vid::getkeys())  loop=0;
		// cls
		SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 255,0,0));

		r={20,20,40,40};
		SDL_FillRect(screen, &r, SDL_MapRGB(screen->format, 255,255,0));
		r={40,40,40,40};
		SDL_FillRect(screen, &r, SDL_MapRGB(screen->format, 0,0,255));

		// blit test
		SDL_Rect dst={ 100, 100 };
		SDL_BlitSurface(box, NULL, screen, &dst);
		// dst={ 200, 200 };
		// SDL_BlitSurface(box2, NULL, screen, &dst);
		
		vid::scaleto(screen, screen);
		SDL_Flip(screen);
		SDL_Delay(16);  // delay
	}

	SDL_FreeSurface(box);
	SDL_Quit();
	return 0;
}