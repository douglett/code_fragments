#pragma once

#include <SDL.h>
#include <string>

namespace gfx {
	const int VERSION=1;
	const std::string INFO="required: SDL 1.2";
	SDL_Surface* mksprite(int w, int h);
	int rmsprite(SDL_Surface* s);
	int rmall();
	int scale2x(SDL_Surface* src, SDL_Surface* dst);
	int flip();
	int drawc(uint32_t color);
	int drawpx(SDL_Surface* dst, int x, int y);
	int drawline(SDL_Surface* dst, int x1, int y1, int x2, int y2);
}