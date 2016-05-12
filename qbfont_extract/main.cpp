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
		auto b = bitset<8>(c[i]);
		for (int j = 0; j < b.size(); j++)
			cout << ( b[j] ? '#' : ' ' );
	}
	cout << endl << endl;
	
	// show data in hex v1
	for (int i = 0; i < bufsize; i++) {
		if (i % 16 == 0)
			cout << endl;
		printf( "%02x ", c[i] );
		// printf( "%#04x, ", c[i] );
	}
	cout << endl << endl;

	// show data in hex v2
	for (int i = 0; i < bufsize; i += 4) {
		if (i % (8*4) == 0)
			cout << endl;
		printf("0x");
		for (int j = 0; j < 4; j++)
			printf( "%02x", c[i+j] );
		printf(", ");
	}
	cout << endl << endl;


	SDL_UnlockSurface(s);

	SDL_Quit();
}
