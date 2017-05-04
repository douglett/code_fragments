#include "globals.h"
#include <iostream>
#include <SDL.h>

using namespace std;


namespace vid {
	const int SURFACE_MAX=1024;
	SDL_Surface* screen=NULL;
	Uint32 tcolor=0;

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

} // end vid


int main(int argc, char** argv) {
	printf("hello world\n");
	vid::init();
	SDL_Surface* screen=vid::screen;
	SDL_Surface* box = vid::makesurface(100, 100);
	SDL_Rect r2={30,30,40,40};
	SDL_FillRect(box, &r2, vid::tcolor);

	// SDL_SetColorKey( surface, SDL_SRCCOLORKEY, SDL_MapRGB(surface->format, 255, 0, 255) );
	
	SDL_Event e;
	int loop=1;
	while (loop) {
		while (SDL_PollEvent(&e)) {
			if (e.type==SDL_QUIT)  loop=0;
			else if (e.type==SDL_KEYDOWN)
				switch (e.key.keysym.sym) {
				case SDLK_ESCAPE:  loop=0;  break;
				default:  break; // nil
				}
		}
		// cls
		SDL_FillRect(screen, NULL, SDL_MapRGBA(screen->format, 255,0,0,255));

		SDL_Rect r={20,20,40,40};
		SDL_FillRect(screen, &r, SDL_MapRGBA(screen->format, 255,255,0,120));
		r.x=r.y=40;
		SDL_FillRect(screen, &r, SDL_MapRGBA(screen->format, 0,0,255,120));

		// blit test
		SDL_Rect dst={ 100, 100, box->clip_rect.w, box->clip_rect.h };
		SDL_BlitSurface(box, NULL, screen, &dst);
		// printf("blit: %d\n", blit);
		
		SDL_Flip(screen);
		SDL_Delay(16);  // delay
	}

	SDL_FreeSurface(box);
	SDL_Quit();
	return 0;
}