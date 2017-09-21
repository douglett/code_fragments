#include "graphics/graphics.h"

using namespace std;


int main(int argc, char** argv) {
	printf("start\n");

	gfx::init(640, 480, "win");

	while (true) {
		SDL_FillRect(SDL_GetVideoSurface(), NULL, 0x000000ff);
		if (gfx::flip())  break;
	}

	SDL_Quit();
	return 0;
}