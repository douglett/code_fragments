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

	// show some info
	printf("%d  %d  %d\n", s->w, s->h, s->pitch);
	uint32_t* data = (uint32_t*)s->pixels;
	
	// for (int y = 0; y < s->h; y++) {
	// 	for (int x = 0; x < s->w; x++)
	// 		cout << ( data[ s->w * y + x ] == 0 ? ' ' : '#' );
	// 	cout << endl;
	// }
	// cout << endl;

	// create char buffer
	int bufsize = s->w * s->h / 8;
	uint8_t c[ bufsize ];

	// load data into buffer
	for (int j = 0; j < bufsize; j += 1) {
		uint32_t* d = data + j*8;
		uint8_t v = 0;
		for (int i = 0; i < 8; i++)
			v |= ( !!d[i] << (7-i) );
		c[j] = v;
	}
	
	// show data in binary
	for (int i = 0; i < bufsize; i++) {
		if (i % 16 == 0)
			cout << endl;
		cout << bitset<8>(c[i]);
	}
	cout << endl << endl;
	
	// show data in hex
	for (int i = 0; i < bufsize; i++) {
		if (i % 16 == 0)
			cout << endl;
		printf( "%02x", c[i] );
	}
	cout << endl << endl;


	SDL_UnlockSurface(s);

	SDL_Quit();
}
