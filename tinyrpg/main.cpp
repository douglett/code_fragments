#include <iostream>
#include "libsrc/xcengine.h"

using namespace std;


int main() {
	cout << "hello world" << endl;
	if (game::init())
		return 1;

	SDL_RenderClear(game::ren);
	SDL_SetRenderDrawColor(game::ren, 255, 150, 150, 255);

	SDL_Rect r = { 10, 10, 10, 10 };
	for (int i = 0; i < 10; i++) {
		r.x = i * 10;
		SDL_RenderDrawRect(game::ren, &r);
	}

	r.x = r.y = -5;
	SDL_RenderDrawRect(game::ren, &r);

	SDL_RenderPresent(game::ren);
	game::waitkey();

	game::quit();
}
