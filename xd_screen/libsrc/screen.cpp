#include <vector>
#include <memory>
#include <iostream>
#include <SDL.h>
#include "screen.h"

using namespace std;


namespace xd {
namespace screen {

	// local consts
	// static const SDL_Rect
	// 		WINRECT = { 0, 0, WIDTH, HEIGHT };

	// member vars
	SDL_Window* win = NULL;
	SDL_Renderer* ren = NULL;
	Sprite* backbuffer = NULL;
	vector<shared_ptr<Sprite> > spritelist;


	int init() {
		// setup SDL
		int init = SDL_Init(SDL_INIT_VIDEO);
		win = SDL_CreateWindow( "xd",
			SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			WIDTH * WIN_SCALE, HEIGHT * WIN_SCALE, 
			0 );
		ren = SDL_CreateRenderer( win, -1, 
			SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
		// error checking
		if (init != 0 || win == NULL || ren == NULL) {
			cerr << SDL_GetError() << endl;
			return 1;
		}
		// initial state
		SDL_RenderSetLogicalSize(ren, WIDTH, HEIGHT);
		cls();
		// setup screen backbuffer in first slot
		makesprite(WIDTH, HEIGHT);
		backbuffer = spritelist[0].get();
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


	// private helper for flip()
	static int clearpoll() {
		SDL_Event e;
		int rval = 0;
		while (SDL_PollEvent(&e))
			if (e.type == SDL_QUIT)
				rval = 1;
		return rval;
	}


	int flip() {
		int rval = clearpoll();  // clear leftover SDL poll events before delay
		SDL_RenderPresent(ren);
		// SDL_Delay(16);  // vsync takes care of this
		return rval;  // return 1 on window quit event
	}


	// repaint all sprites and flip screen
	int paint() {
		cls();
		for (auto& spr : spritelist)
			spr->flip();
		return flip();  // return 1 on window quit event
	}


	Sprite* makesprite(int width, int height) {
		auto s = make_shared<Sprite>(width, height);
		spritelist.push_back(s);
		return s.get();
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
