#include <iostream>
#include <SDL.h>
#include <SDL_image.h>

using namespace std;


int extract1();
int extract2();
SDL_Surface* s = NULL;
uint32_t* data = NULL;


int main() {
	cout << "hello world" << endl;
	SDL_Init(SDL_INIT_VIDEO);
	IMG_Init(0);

	// load image
	s = IMG_Load("qbfont.png");
	SDL_LockSurface(s);
	// show some info
	printf("%d  %d  %d\n", s->w, s->h, s->pitch);
	data = (uint32_t*)s->pixels;

	// extract1();
	extract2();

	SDL_UnlockSurface(s);
	SDL_Quit();
}


int extract2() {
	static const int 
		rotate = 0,
		test_display = 1;

	int pitch = s->w;
	int start = 0, last = 256;
	// if (test_display)  start = 'a',  last = 'e';
	
	for (int i = start; i < last; i++) {
		int yoff = (i * 8) / pitch * 8;
		int xoff = (i * 8) % pitch;
		uint8_t c[8] = { 0 };

		if (rotate)
			// rotated 90 degrees right
			for (int x = 0; x < 8; x++) {
				for (int y = 7; y >= 0; y--) {
					int v = !!data[ (yoff+y) * pitch + xoff + x ];
					c[x] = c[x] << 1 | v;
					if (test_display)  cout << ( v ? '#' : ' ' );
				}
				if (test_display)  cout << endl;
			}
		else
			// normal
			for (int y = 0; y < 8; y++) {
				for (int x = 0; x < 8; x++) {
					int v = !!data[ (yoff+y) * pitch + xoff + x ];
					c[y] = c[y] << 1 | v;
					if (test_display)  cout << ( v ? '#' : ' ' );
				}
				if (test_display)  cout << endl;
			}

		for (int j = 0; j < 8; j += 2)
			printf("0x%02x%02x,", c[j], c[j+1]);
		// printf( i % 2 == 1 ? "\n" : " " );
		printf("\n");
	}
	
	return 0;
}


int extract1() {
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
	return 0;
}
