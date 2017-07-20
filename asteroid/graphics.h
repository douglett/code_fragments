#pragma once

#include <SDL.h>
#include <string>

namespace gfx {
	const int VERSION=1, ERROR_EXIT=1;
	const std::string INFO="required: SDL 1.2";
	extern SDL_Surface* font;
	int init(int w, int h, std::string winname);
	int flip();
	// sprites
	SDL_Surface* mksprite(int w, int h);
	int rmsprite(SDL_Surface* s);
	int rmall();
	int scale2x(SDL_Surface* src, SDL_Surface* dst);
	// shapes
	uint32_t drawc(uint32_t color);
	uint32_t drawc(uint8_t r, uint8_t g, uint8_t b);
	int drawpx(SDL_Surface* dst, int x, int y);
	int drawline(SDL_Surface* dst, int x1, int y1, int x2, int y2);
	// font
	int mkfont();
	SDL_Rect getc(int c);
	int drawstr(SDL_Surface* dst, int x, int y, const std::string& str);
}
