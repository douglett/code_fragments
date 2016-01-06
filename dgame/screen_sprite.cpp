#include <algorithm>
#include "screen.h"


using namespace std;


namespace xd {
namespace screen {

// constructor
Sprite::Sprite(int width, int height) {
	data = new uint32_t[width*height+2];
	size.w = dst.w = data[0] = width;   // set widths
	size.h = dst.h = data[1] = height;  // set heights
	fill_n(data+2, width*height, rgb(0, 0, 0));  // fill black
	tex = SDL_CreateTexture(screen::ren,  // make texture
		SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 
		width, height);
}


uint32_t* Sprite::getdata() {
	dirty = 1;
	return data;
}


int Sprite::pos(int x, int y) {
	dst.x = x;
	dst.y = y;
	return 0;	
}


// write texture to GPU
int Sprite::stream() {
	uint32_t* pix; 	// holds pixel data
	int pitch;	    // holds width*bytelen (unused)
	SDL_LockTexture(tex, NULL, (void**)&pix, &pitch);  // prepare texture data
	memcpy(pix, data+2, sizeof(uint32_t)*size.w*size.h);  // copy pixels to GPU
	SDL_UnlockTexture(tex);  // done copying
	dirty = 0;
	return 0;
}


int Sprite::flip() {
	if (dst.x >= screen::WIDTH || dst.y >= screen::HEIGHT 
			|| dst.x + dst.w < 0 || dst.y + dst.h < 0)
		return 0;
	if (dirty)
		stream();
	SDL_RenderCopy(screen::ren, tex, NULL, &dst);
	return 0;
}

} // end screen
} // end xd