#include "globals.h"
#include <iostream>
#include <SDL.h>

using namespace std;


int makesprites();
SDL_Surface* screen = NULL;
SDL_Surface* ship = NULL;


int main(int argc, char** argv) {
	printf("start\n");
	int err = SDL_Init(SDL_INIT_VIDEO);
	if (err!=0) {
		fprintf(stderr, "error loading SDL: %d\n", err);
		return 1;
	}
	screen = SDL_SetVideoMode(640, 480, 32, SDL_HWSURFACE);
	if (screen==NULL) {
		fprintf(stderr, "error: could not load video\n");
		return 1;
	}

	makesprites();

	int doloop=1;
	SDL_Event e;
	while (doloop) {
		SDL_FillRect(screen, NULL, 0x000000ff);
		SDL_Rect pos={10,10};
		SDL_BlitSurface(ship, NULL, screen, &pos);
		SDL_Flip(screen);
		SDL_Delay(16);

		while (SDL_PollEvent(&e))
			switch (e.type) {
			case SDL_QUIT:  doloop=0;  break;
			case SDL_KEYDOWN:
				switch (e.key.keysym.sym) {
				case SDLK_ESCAPE:  doloop=0;  break;
				case SDLK_LEFT:  printf("l\n");  break;
				case SDLK_RIGHT:  printf("r\n");  break;
				case SDLK_UP:  printf("r\n");  break;
				case SDLK_DOWN:  printf("r\n");  break;
				default:  printf("%d\n", e.key.keysym.sym);
				}
				break;
			}
	}

	SDL_Quit();
	return 0;
}


int makesprites() {
	ship = SDL_CreateRGBSurface(SDL_HWSURFACE, 32, 32, 32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
	SDL_LockSurface(ship);
	SDL_FillRect(ship, NULL, 0xffffffff);
	SDL_UnlockSurface(ship);
	return 0;
}