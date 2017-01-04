#include <cassert>
#include "game.h"

using namespace std;


namespace draw {
	// predef
	static void draw_eyebg();
	static void make_tiles();
	// consts
	static const uint32_t 
		COL_G1 = 0x00cc00ff,
		COL_G2 = 0x005500ff;
	// vars
	uint32_t* tiles[4][3] = { {NULL} };
	xd::screen::Sprite 
		*eye = NULL,  
		*eyebg = NULL, 
		*compass = NULL,  
		*vmap = NULL;


	void init() {
		// make sprites
		eye = xd::screen::makesprite(100, 100, "eye");
		eye->pos(10, 10);
		eye->zindex(10);
		compass = xd::screen::makesprite(40, 40, "compass");
		compass->pos(120, 10);
		vmap = xd::screen::makesprite(40, 40, "vmap");
		vmap->pos(120, 60);
		eyebg = xd::screen::makesprite(102, 102, "eyebg");
		eyebg->pos(9, 9);
		// make all tiles
		make_tiles();
		// first draw functions
		draw_eyebg();
	}

	static void draw_eyebg() {
		auto* dat = eyebg->getdata();
		int  w = dat[0],  h = dat[1];
		// xd::draw::clear(dat, 0x222222ff);
		xd::draw::clear(dat, 0x000000ff);
		uint32_t c = COL_G2;
		for (int i = 0, j = 9; i < h/2; i += j, j--) {
			xd::draw::traceline(dat, 0, i, w, i, c);
			xd::draw::traceline(dat, 0, h-1-i, w, h-1-i, c);
		}
		xd::draw::traceline(dat, 0, 0, 0, h, c);
		xd::draw::traceline(dat, w-1, 0, w-1, h, c);
	}

	static void make_tiles() {
		static uint32_t* unknown = NULL;
		// simple rerun test
		assert(unknown == NULL);
		unknown = xd::draw::make_img(10, 10);
		xd::draw::clear(unknown, 0xff00ffff);
		// 
		// uint32_t c = 0xaa0000ff, b = 0x999999ff;
		uint32_t c = COL_G1, b = 0x000000ff;
		uint32_t* dat;
		// row-0 : left
		dat = tiles[0][0] = xd::draw::make_img(14, 100);
		xd::draw::clear(dat);
		xd::draw::tracepoly(dat, 0, 0,  {{0, 0}, {13, 13}, {13, 86}, {0, 99}},  c);
		xd::draw::fill(dat, 1, 2, b);
		// row-0 : mid
		tiles[0][1] = unknown;
		// row-0 : right
		tiles[0][2] = dupflip(tiles[0][0]);
		// row-1 : left
		dat = tiles[1][0] = xd::draw::make_img(30, 74);
		xd::draw::clear(dat);
		xd::draw::tracepoly(dat, 13, 0,  {{0, 0}, {15, 15}, {15, 58}, {0, 73}, {0, 0}, {-14, 0}, {-14, 73}, {0, 73}},  c);
		xd::draw::fill(dat, 1, 2, b);
		xd::draw::fill(dat, 14, 3, b);
		// row-1 : mid
		dat = tiles[1][1] = xd::draw::make_img(74, 74);
		xd::draw::clear(dat);
		xd::draw::tracerect(dat, 0, 0, 74, 74, c);
		xd::draw::fill(dat, 1, 2, b);
		// row-1 : right
		tiles[1][2] = dupflip(tiles[1][0]);
		// row-2 : left
		dat = tiles[2][0] = xd::draw::make_img(38, 44);
		xd::draw::clear(dat);
		xd::draw::tracepoly(dat, 28, 0, {{0, 0}, {9, 9}, {9, 34}, {0, 43}, {0, 0}, {-43, 0}, {-43, 43}, {0, 43}}, c);
		xd::draw::fill(dat, 1, 3, b);
		xd::draw::fill(dat, 29, 3, b);
		// row-2 : mid
		dat = tiles[2][1] = xd::draw::make_img(44, 44);
		xd::draw::clear(dat);
		xd::draw::tracerect(dat, 0, 0, 44, 44, c);
		xd::draw::fill(dat, 1, 2, b);
		// row-2 : right
		tiles[2][2] = dupflip(tiles[2][0]);
		// row-3 : left
		tiles[3][0] = unknown;
		// row-3 : mid
		dat = tiles[3][1] = xd::draw::make_img(26, 26);
		xd::draw::fillrect(dat, 0, 0, 26, 26, b);
		xd::draw::tracerect(dat, 0, 0, 26, 26, c);
		// row-3 : right
		tiles[3][2] = unknown;
	}

	void far_row(uint32_t* dat) {
		string row = gmap::getrow(3);
		printf("far_row  : [%s]\n", row.c_str());
		for (int i = 1; i <= 3; i++)
			if (row[i] == '.')  xd::draw::blit(tiles[3][1], dat, 37+(25*(i-2)), 37);
		// draw edges only if flush
		if (row[0] == '.' && row[1] == '.')  xd::draw::blit(tiles[3][1], dat, 37+(25*-2), 37);
		if (row[4] == '.' && row[3] == '.')  xd::draw::blit(tiles[3][1], dat, 37+(25* 2), 37);
	}

	void back_row(uint32_t* dat) {
		string row = gmap::getrow(2);
		printf("back_row : [%s]\n", row.c_str());
		if (row[1] == '.')  xd::draw::blit(tiles[2][0], dat, 0, 28);  // left row 2
		if (row[3] == '.')  xd::draw::blit(tiles[2][2], dat, 99-28-9, 28);  // right row 2
		if (row[2] == '.')  xd::draw::blit(tiles[2][1], dat, 28, 28);  // mid row 2
	}

	void mid_row(uint32_t* dat) {
		string row = gmap::getrow(1);
		printf("mid_row  : [%s]\n", row.c_str());
		if (row[1] == '.')  xd::draw::blit(tiles[1][0], dat, 0, 13);  // left row 1
		if (row[3] == '.')  xd::draw::blit(tiles[1][2], dat, 70, 13);  // right row 1
		if (row[2] == '.')  xd::draw::blit(tiles[1][1], dat, 13, 13);  // mid row 1
	}

	void front_row(uint32_t* dat) {
		string row = gmap::getrow(0);
		printf("front_row: [%s]\n", row.c_str());
		if (row[1] == '.')  xd::draw::blit(tiles[0][0], dat, 0, 0);  // left row 0
		if (row[3] == '.')  xd::draw::blit(tiles[0][2], dat, 86, 0);  // right row 0
	}

	void draw_vmap() {
		// sizing
		const int  spc = 3,  mapw = gmap::gmap[0].length(),  maph = gmap::gmap.size();
		auto* dat = vmap->getdata();
		// clear
		xd::draw::clear(dat, 0x000099ff);
		// xd::draw::tracerect(dat, 0, 0, mapw*spc + spc, maph*spc + spc, 0xff0000ff);
		draw_border(dat, 0, 0, mapw*spc + spc, maph*spc + spc, 0xff0000ff);
		// repaint
		for (int y = 0; y < maph; y++)
			for (int x = 0; x < mapw; x++)
				if (gmap::gmap[y][x] == ' ') {
					uint32_t c = 0xffffffff;
					if (x == gmap::posx && y == gmap::posy)  c = 0xff00ffff;  // player pos
					xd::draw::fillrect(dat, 1 + spc*x, 1 + spc*y, spc, spc, c);
				}
	}

	void draw_compass() {
		auto* dat = compass->getdata();
		int w = dat[0], h = dat[1];
		// uint32_t c = 0xffffffff, c2 = 0xaaaaaaff;
		uint32_t  c = COL_G1,  c2 = COL_G2;
		xd::draw::clear(dat);
		// border
		draw_border(dat, 0, 0, w, h, c2);
		draw_border(dat, 1, 1, w-2, h-2, c);
		// xd::draw::fill(dat, 3, 3, 0x770000ff);
		// mid cross
		xd::draw::traceline(dat, w/2-5, h/2, w/2+5, h/2, c);
		xd::draw::traceline(dat, w/2, h/2+5, w/2, h/2-5, c);
		// north dir
		switch (gmap::dir) {
		case 0:  xd::text::print(dat, "N", w/2 - 4, h/2 - (8+8), c);  break;  // north
		case 1:  xd::text::print(dat, "N", w/2 - (8+8), h/2 - 4, c);  break;  // east
		case 2:  xd::text::print(dat, "N", w/2 - 4, h/2 + 8+2, c);    break;  // south
		case 3:  xd::text::print(dat, "N", w/2 + 8+2, h/2 - 4, c);    break;  // west
		}
	}



/*** helpers ***/

	uint32_t* dupflip(const uint32_t* src) {
		auto* dat = xd::draw::make_img(src[0], src[1]);
		xd::draw::clear(dat);
		xd::draw::blit(src, dat, 0, 0);
		xd::draw::flip_h(dat);
		return dat;
	}

	// static vector<pair<int, int>> getborder(int w, int h) {
	// 	return {
	// 		{1, 0}, {w-2, 0}, {w-2, 1}, {w-1, 1}, 
	// 		{w-1, h-2}, {w-2, h-2}, {w-2, h-1},
	// 		{1, h-1}, {1, h-2}, {0, h-2}, 
	// 		{0, 1}, {1, 1}
	// 	};
	// }
	void draw_border(uint32_t* dat, int x, int y, int w, int h, uint32_t c) {
		xd::draw::tracepoly(dat, x, y,  {
			{1, 0}, {w-2, 0}, {w-2, 1}, {w-1, 1}, 
			{w-1, h-2}, {w-2, h-2}, {w-2, h-1},
			{1, h-1}, {1, h-2}, {0, h-2}, 
			{0, 1}, {1, 1}
		},  c);
	}

} // end draw