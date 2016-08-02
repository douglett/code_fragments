#include <iostream>
#include <SDL.h>
#include <SDL_image.h>

using namespace std;


int main() {
	cout << "hello world" << endl;

	SDL_Init(SDL_INIT_VIDEO);
	IMG_Init(0);
	SDL_Surface* s = IMG_Load("ansi81.bmp");
	SDL_LockSurface(s);

	// show some info
	printf("%d  %d  %d\n", s->w, s->h, s->pitch);
	printf("format: %d \n", s->format->BitsPerPixel);
	
	// uint32_t* data = (uint32_t*)s->pixels;
	uint8_t* data = (uint8_t*)s->pixels;

	for (int y = 8*1; y < 8*2; y++) {
		for (int i = 0; i < 4*6; i++) {
			// printf("%02x ", data[y*s->w + i]);
			cout << ( data[y*s->w + i] ? '#' : ' ' );
		}
		cout << endl;
	}

	SDL_UnlockSurface(s);
	SDL_Quit();
}