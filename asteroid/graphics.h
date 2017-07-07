#pragma once

#include <SDL.h>
#include <string>

namespace gfx {
	const int VERSION=1;
	const std::string INFO="required: SDL 1.2";
	SDL_Surface* mksprite(int w, int h);
	int rmsprite(SDL_Surface* s);
	int scale2x(SDL_Surface* src, SDL_Surface* dst);
	int flip();
}