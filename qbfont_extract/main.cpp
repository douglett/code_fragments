#include <iostream>
#include <SDL.h>
#include <SDL_image.h>

using namespace std;


int main() {
	cout << "hello world" << endl;

	SDL_Init(SDL_INIT_VIDEO);
	IMG_Init(0);

	SDL_Surface* s = IMG_Load("qbfont.png");

	SDL_LockSurface(s);

	// stuff
	printf("%d  %d  %d\n", s->w, s->h, s->pitch);

	uint32_t* data = (uint32_t*)s->pixels;
	for (int y = 0; y < s->h; y++) {
		for (int x = 0; x < s->w; x++)
			cout << ( data[ s->w * y + x ] == 0 ? ' ' : '#' );
		cout << endl;
	}


	SDL_UnlockSurface(s);

	SDL_Quit();
}
