#include <iostream>
#include <SDL.h>
#include <SDL_image.h>

using namespace std;


void showchar(char* c) {
	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 4; x++)
			cout << ( c[y*4 + x] ? '#' : ' ' );
		cout << endl;
	}
}


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

	// for (int y = 8*1; y < 8*2; y++) {
	// 	for (int i = 0; i < 4*26; i++) {
	// 		// printf("%02x ", data[y*s->w + i]);
	// 		cout << ( data[y*s->w + i] ? '#' : ' ' );
	// 	}
	// 	cout << endl;
	// }

	int xoff = 0;
	int yoff = 0;
	char c[4*8];
	for (int i = 0; i < 256; i++) {
		
		// make char sprite
		for (int y = 0; y < 8; y++)
			for (int x = 0; x < 4; x++)
				c[4*y + x] = !!data[ (yoff+y)*s->w + xoff+x ];

		// show char
		// showchar(c);

		// dump to terminal
		cout << endl;
		for (int x = 0; x < 4; x += 2) {
			uint8_t n1 = 0;
			uint8_t n2 = 0;
			for (int y = 7; y >= 0; y--) {
				n1 |= c[y*4 + x] << y;
				n2 |= c[y*4 + x+1] << y;
			}
			printf("0x%02x%02x, ", n1, n2);
		}

		// increment
		xoff += 4;
		if (i > 0 && i % 64 == 0) {
			xoff = 0;
			yoff += 8;
		}
	}

	cout << endl << endl;

	SDL_UnlockSurface(s);
	SDL_Quit();
}