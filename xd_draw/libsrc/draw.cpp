#include <stdint.h>
#include <memory>
#include <algorithm>
#include <cassert>

#include "draw.h"

using namespace std;


namespace xd {
namespace draw {

	uint32_t rgb(uint8_t r, uint8_t g, uint8_t b) {
		return (r<<24) | (g<<16) | (b<<8) | 255;
	}

	uint32_t* make_img(int w, int h) {
		assert(w > 0 && h > 0);
		uint32_t* img = new uint32_t[w*h+2];  // raw ptr
		img[0] = w;
		img[1] = h;
		fill_n(img+2, w*h, rgb(0, 0, 0));  // black image
		return img;
	}

	shared_ptr<uint32_t> make_shared_img(int w, int h) {
		uint32_t* img = make_img(w, h);
		auto img_shared = shared_ptr<uint32_t>(
			img, default_delete<uint32_t[]>() );  // make managed ptr
		return img_shared;
	}

	int fillrect(uint32_t* imgdata, uint32_t color, int posx, int posy, int w, int h) {
		int img_width = imgdata[0];
		int img_height = imgdata[1];
		for (int y = max(posy, 0); y < min(posy+h, img_height); y++)
			for (int x = max(posx, 0); x < min(posy+w, img_width); x++)
				imgdata[img_width*y + x + 2] = color;
		return 0;
	}

	int tracerect(uint32_t* imgdata, uint32_t color, int posx, int posy, int w, int h) {
		int img_width = imgdata[0];
		int img_height = imgdata[1];
		int maxw = posx + w - 1;
		int maxh = posy + h - 1;
		if (posy >= 0)
			for (int x = max(posx, 0); x < min(posy+w, img_width); x++)
				imgdata[2 + x] = color;
		if (maxh < img_height)
			for (int x = max(posx, 0); x < min(posy+w, img_width); x++)
				imgdata[2 + img_width*maxh + x] = color;
		if (posx >= 0)
			for (int y = max(posy, 0); y < min(posy+h, img_height); y++)
				imgdata[2 + img_width*y] = color;
		if (maxw < img_width)
			for (int y = max(posy, 0); y < min(posy+h, img_height); y++)
				imgdata[2 + img_width*y + maxw] = color;
		return 0;
	}

	int traceline(uint32_t* imgdata, uint32_t color, int x1, int y1, int x2, int y2) {
		int img_width = imgdata[0];
		int img_height = imgdata[1];
		if (x1 > x2) swap(x1, x2);
		if (y1 > y2) swap(y1, y2);
		int dx = x2 - x1;
		int dy = y2 - y1;
		int steps = max(dx, dy);
		double stepx = dx / double(steps);
		double stepy = dy / double(steps);

		for (int i = 0; i < steps; i++) {
			int x = x1 + int(stepx * i);
			int y = y1 + int(stepy * i);
			if (x > 0 && y > 0 && x < img_width && y < img_height)
				imgdata[2 + img_width*y + x] = color;
		}
		
		return 0;
	}

	int blit(uint32_t* src, uint32_t* dst, int posx, int posy) {
		int src_width = src[0],
			src_height = src[1],
			dst_width = dst[0],
			dst_height = dst[1];
		for (int y = 0; y < src_height; y++) {
			if (posy+y < 0 || posy+y >= dst_height)
				continue;
			for (int x = 0; x < src_width; x++) {
				if (posx+x < 0 || posx+x >= dst_width)
					continue;
				dst[dst_width*(posy+y) + (posx+x) + 2] = src[src_width*y + x + 2];
			}
		}
		return 0;
	}

} // end draw
} // end xd
