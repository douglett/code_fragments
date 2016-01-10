#include <iostream>
#include <memory>
#include <algorithm>
#include <cassert>
#include "xd_screen/screen.h"

using namespace std;


namespace draw {

	uint32_t rgb(uint8_t r, uint8_t g, uint8_t b) {
		return (r<<24) | (g<<16) | (b<<8) | 255;
	}

	shared_ptr<uint32_t> make_img(int w, int h) {
		assert(w > 0 && h > 0);
		uint32_t* img = new uint32_t[w*h+2];  // raw ptr
		auto img_shared = shared_ptr<uint32_t>(img, default_delete<uint32_t[]>());  // managed ptr
		img[0] = w;
		img[1] = h;
		fill_n(img+2, w*h, rgb(0, 0, 0));  // black image
		return img_shared;
	}

	int fillrect(uint32_t* imgdata, uint32_t col, int posx, int posy, int w, int h) {
		int imgw = imgdata[0];
		int imgh = imgdata[1];
		for (int y = max(posy, 0); y < min(posy+h, imgh); y++)
			for (int x = max(posx, 0); x < min(posy+w, imgw); x++)
				imgdata[imgh*y + x + 2] = col;
		return 0;
	}

} // end draw


int main() {
	cout << "hello world" << endl;

	xd::screen::init();

	// uint32_t* img = new uint32_t[20*20+2];
	// delete[] img;
	auto imgres = draw::make_img(20, 20);
	auto* img = imgres.get();
	draw::fillrect(img, draw::rgb(255, 0, 0), 0, 0, 20, 20);
	draw::fillrect(xd::screen::backbuffer->getdata(), draw::rgb(255, 0, 0), 0, 0, 40, 40);

	xd::screen::paint();
	SDL_Delay(1000);

	xd::screen::quit();
}