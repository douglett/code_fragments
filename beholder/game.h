#pragma once

#include <vector>
#include <string>
#include "xdlib/xdlib.h"

namespace gmap {
	extern int  posx, posy, dir;
	extern std::vector<std::string> gmap;
	int is_empty(char c);
	int is_null(char c);
	int is_nothing(char c);
	std::string getrow(int row);
} // end gmap

namespace gdraw {
	extern uint32_t* tiles[4][3];
	extern xd::screen::Sprite  *eye, *eyebg, *compass, *vmap;
	void init();
	void far_row(uint32_t* dat);
	void back_row(uint32_t* dat);
	void mid_row(uint32_t* dat);
	void front_row(uint32_t* dat);
	void draw_vmap();
	void draw_compass();
	// helpers
	uint32_t* dupflip(const uint32_t* src);
	void      draw_border(uint32_t* dat, int x, int y, int w, int h, uint32_t c);
} // end gdraw