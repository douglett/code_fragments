#pragma once

#include <stdint.h>

namespace xd {
namespace draw {
	uint32_t* make_img(int w, int h);
	// shared_ptr<uint32_t> make_shared_img(int w, int h);
	uint32_t rgb(uint8_t r, uint8_t g, uint8_t b);
	int fillrect(uint32_t* imgdata, uint32_t color, int posx, int posy, int w, int h);
	int blit(uint32_t* src, uint32_t* dst, int posx, int posy);
} // end draw
} // end xd
