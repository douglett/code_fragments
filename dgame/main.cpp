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


	// int color converters
	uint32_t rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
		return (r<<24) | (g<<16) | (b<<8) | a;
	}
	uint32_t rgb(uint8_t r, uint8_t g, uint8_t b) {
		return (r<<24) | (g<<16) | (b<<8) | 255;
	}

} // end screen
} // end xd



using namespace xd;
using namespace xd::screen;


// GPU Sprite
class Sprite {
private:
	int dirty = 1;
	uint32_t* data = NULL;
	SDL_Texture* tex = NULL;
	SDL_Rect size = { 0 };
public:
	SDL_Rect dst = { 0 };  // user editable
public:
	// constructor
	Sprite(int width, int height) {
		data = new uint32_t[width*height+2];
		size.w = dst.w = data[0] = width;   // set widths
		size.h = dst.h = data[1] = height;  // set heights
		fill_n(data+2, width*height, rgb(0, 0, 0));  // fill black
		tex = SDL_CreateTexture(screen::ren,  // make texture
			SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 
			width, height);
	}
	uint32_t* getdata() {
		dirty = 1;
		return data;
	}
	int stream() {
		uint32_t* pix; 	// holds pixel data
		int pitch;	    // holds width*bytelen (unused)
		SDL_LockTexture(tex, NULL, (void**)&pix, &pitch);  // prepare texture data
		memcpy(pix, data+2, sizeof(uint32_t)*size.w*size.h);  // copy pixels to GPU
		SDL_UnlockTexture(tex);  // done copying
		dirty = 0;
		return 0;
	}
	int flip() {
		if (dst.x >= screen::WIDTH || dst.y >= screen::HEIGHT 
				|| dst.x + dst.w < 0 || dst.y + dst.h < 0)
			return 0;
		if (dirty)
			stream();
		SDL_RenderCopy(screen::ren, tex, NULL, &dst);
		return 0;
	}
};


int main() {
	cout << "hello world" << endl;
	screen::init();

	// box test
	SDL_Rect r = { 5, 5, 10, 10 };
	SDL_SetRenderDrawColor(screen::ren, 255, 0, 0, 255);
	SDL_RenderFillRect(screen::ren, &r);

	// sprite test
	Sprite s(64, 64);
	s.dst.x = 20;
	s.dst.y = 20;
	uint32_t* d = s.getdata();
	fill_n(d+2, d[0]*d[1], rgb(0, 255, 0));
	s.flip();

	screen::flip();
	SDL_Delay(2000);

	screen::quit();
}
