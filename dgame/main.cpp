#include <iostream>
#include "SDL.h"

using namespace std;


namespace dgame {
namespace screen {

	// consts
	const int 
			WIDTH = 240,
			HEIGHT = 160,
			WIN_SCALE = 2;
	static const SDL_Rect
			WINRECT = { 0, 0, WIDTH, HEIGHT };

	// member vars
	SDL_Window* win = NULL;
	SDL_Renderer* ren = NULL;


	int init() {
		SDL_Init(SDL_INIT_VIDEO);
		SDL_CreateWindowAndRenderer(WIDTH * WIN_SCALE, HEIGHT * WIN_SCALE, 0, &win, &ren);
		if (ren == NULL || win == NULL)
			return 1;
		SDL_RenderSetLogicalSize(ren, WIDTH, HEIGHT);
		SDL_RenderClear(ren);
		return 0;
	}


	int quit() {
		SDL_Quit();
		return 0;
	}


	int cls() {
		SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
		SDL_RenderFillRect(ren, &WINRECT);
		return 0;
	}


	int flip() {
		SDL_RenderPresent(ren);
		SDL_Delay(16);
		return 0;
	}

} // end screen
} // end dgame



using namespace dgame;


int main() {
	cout << "hello world" << endl;
	screen::init();

	SDL_Rect r = { 5, 5, 10, 10 };
	SDL_SetRenderDrawColor(screen::ren, 255, 0, 0, 255);
	SDL_RenderFillRect(screen::ren, &r);

	uint32_t tile[64*64+2] = { 0 };

	screen::flip();
	SDL_Delay(2000);

	screen::quit();
}
