#include <iostream>
#include "SDL.h"

using namespace std;


namespace xd {
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
} // end xd



using namespace xd;


uint32_t rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
	return (r<<24) | (g<<16) | (b<<8) | a;
}
uint32_t rgb(uint8_t r, uint8_t g, uint8_t b) {
	return (r<<24) | (g<<16) | (b<<8) | 255;
}


int main() {
	cout << "hello world" << endl;
	screen::init();

	SDL_Rect r = { 5, 5, 10, 10 };
	SDL_SetRenderDrawColor(screen::ren, 255, 0, 0, 255);
	SDL_RenderFillRect(screen::ren, &r);

	uint32_t tile[64*64+2] = { 64, 64, 0 };
	fill_n(tile+2, 64*64, rgb(0, 0, 255));
	SDL_Texture* tex = SDL_CreateTexture(screen::ren, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 64, 64);
	uint32_t* pix;
	int pitch;
	SDL_LockTexture(tex, NULL, (void**)&pix, &pitch);
	memcpy(pix, tile+2, sizeof(uint32_t)*64*64);
	SDL_UnlockTexture(tex);

	cout << sizeof(uint32_t) << endl;
	cout << (255 << 2) << endl;
	cout << hex << rgba(255, 0, 0, 255) << endl;
	cout << tile[0] << " " << tile[1] << endl;

	SDL_Rect r2 = { 20, 20, 64, 64 };
	SDL_RenderCopy(screen::ren, tex, NULL, &r2);

	screen::flip();
	SDL_Delay(2000);

	screen::quit();
}
