#include <iostream>
#include "libsrc/xcengine.h"

using namespace std;


void draw();


SDL_Texture* sprites = NULL;
SDL_Rect parchment = { 0, 0, 100, 28 };
SDL_Rect cardback = { 0, 29, 16, 18 };
SDL_Rect spade = { 17, 30, 14, 16 };
SDL_Rect heart = { 31, 30, 14, 16 };
SDL_Rect club = { 47, 30, 15, 16 };
SDL_Rect diamond = { 62, 30, 14, 16 };
SDL_Rect man = { 0, 48, 12, 12 };


int main() {
	cout << "hello world" << endl;
	if (game::init())
		return 1;

	// sprite images
	sprites = texture::get("images")->tex;

	draw();
	SDL_RenderPresent(game::ren);
	game::waitkey();

	game::quit();
}


void drawcard(int type, int x, int y) {
	static SDL_Rect cards[] = { spade, heart, club, diamond };

	SDL_Rect dst = cardback;
	dst.x = x;
	dst.y = y;
	SDL_RenderCopy(game::ren, sprites, &cardback, &dst);

	dst = cards[type];
	dst.x = x + 1;
	dst.y = y + 1;
	SDL_RenderCopy(game::ren, sprites, &cards[type], &dst);
}


void draw() {
	// cls
	SDL_SetRenderDrawColor(game::ren, 0, 0, 0, 255);
	SDL_RenderClear(game::ren);

	// game background
	SDL_SetRenderDrawColor(game::ren, 50, 50, 50, 255);
	SDL_Rect scr = { 0, 0, game::width, game::height };
	SDL_RenderFillRect(game::ren, &scr);

	// SDL_SetRenderDrawColor(game::ren, 255, 150, 150, 255);
	// SDL_Rect r = { 10, 10, 10, 10 };
	// for (int i = 0; i < 10; i++) {
	// 	r.x = i * 10;
	// 	SDL_RenderDrawRect(game::ren, &r);
	// }

	// r.x = r.y = -5;
	// SDL_RenderDrawRect(game::ren, &r);

	// draw map
	SDL_Rect dst = { 0, 0, 13, 13 };
	SDL_SetRenderDrawColor(game::ren, 255, 150, 150, 255);
	for (int y = 0; y < 10; y++) {
		dst.y = y * 12 - 4;
		for (int x = 0; x < 10; x++) {
			dst.x = x * 12 - 4;
			SDL_RenderDrawRect(game::ren, &dst);
		}
	}

	dst = man;
	dst.x = 12*4 - 4;
	dst.y = 12*3 - 4;
	SDL_RenderCopy(game::ren, sprites, &man, &dst);

	// draw parchment background
	dst = parchment;
	dst.x = 0;
	dst.y = 71;
	SDL_RenderCopy(game::ren, sprites, &parchment, &dst);

	// draw cards
	// drawcard(0, 22, 74);
	// drawcard(1, 39, 78);
	// drawcard(2, 56, 81);
	// drawcard(3, 73, 80);

	drawcard(0, 22, 77);
	drawcard(1, 39, 77);
	drawcard(2, 56, 77);
	drawcard(3, 73, 77);
}
