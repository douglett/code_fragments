#include <SDL.h>
#include "screen.h"

// using namespace std;


namespace xd {
namespace screen {

	// local consts
	// static const SDL_Rect
	// 		WINRECT = { 0, 0, WIDTH, HEIGHT };
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
		SDL_RenderClear(ren);
		// SDL_RenderFillRect(ren, &WINRECT);
		return 0;
	}


	int flip() {
		SDL_RenderPresent(ren);
		SDL_Delay(16);
		return 0;
	}


	// int color converters
	uint32_t rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
		return (r<<24) | (g<<16) | (b<<8) | a;
	}
	uint32_t rgb(uint8_t r, uint8_t g, uint8_t b) {
		return (r<<24) | (g<<16) | (b<<8) | 255;
	}

} // end screen
} // end xd