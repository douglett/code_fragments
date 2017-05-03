#include "globals.h"
#include <iostream>
#include <SDL.h>

using namespace std;


int main(int argc, char** argv) {
	printf("hello world\n");

	if (SDL_Init(SDL_INIT_VIDEO)) {
		fprintf(stderr, "init error\n");
		return 1;
	}
	SDL_Surface* screen = SDL_SetVideoMode( 640, 480, 32, SDL_SWSURFACE );
	if (screen==NULL) {
		fprintf(stderr, "make surface error\n");
		return 1;
	}
	SDL_WM_SetCaption("mywin1", "mywin2");

	// SDL_SetColorKey( surface, SDL_SRCCOLORKEY, SDL_MapRGB(surface->format, 255, 0, 255) );

	SDL_Event e;
	int loop=1;
	while (loop) {
		while (SDL_PollEvent(&e)) {
			if (e.type==SDL_QUIT)  loop=0;
		}

		SDL_FillRect(screen, NULL, SDL_MapRGBA(screen->format, 255,0,0,255));
		SDL_Rect r={20,20,40,40};
		SDL_FillRect(screen, &r, SDL_MapRGBA(screen->format, 255,255,0,120));
		r.x=r.y=40;
		SDL_FillRect(screen, &r, SDL_MapRGBA(screen->format, 0,0,255,120));
		SDL_Flip(screen);
		SDL_Delay(16);
	}

	SDL_Quit();
	return 0;
}