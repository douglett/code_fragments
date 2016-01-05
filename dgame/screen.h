#ifndef __XD_SCREEN_DEF__
	#define __XD_SCREEN_DEF__

#include <stdint.h>
#include <SDL.h>

namespace xd {
namespace screen {
	// consts
	const int 
			WIDTH = 240,
			HEIGHT = 160,
			WIN_SCALE = 2;
	// member vars
	extern SDL_Window* win;
	extern SDL_Renderer* ren;
	// methods
	int init();
	int quit();
	int cls();
	int flip();
	uint32_t rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
	uint32_t rgb(uint8_t r, uint8_t g, uint8_t b);


	// GPU Sprite class
	class Sprite {
	private:
		int dirty = 1;
		uint32_t* data = NULL;
		SDL_Texture* tex = NULL;
		SDL_Rect size = { 0 };
	public:
		SDL_Rect dst = { 0 };  // user editable
		// methods
		Sprite(int width, int height);
		uint32_t* getdata();
		int stream();
		int flip();
	};
} // end screen
} // end xd

#endif