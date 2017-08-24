#include "graphics/graphics.h"
#include <iostream>
#include <vector>

using namespace std;


const int grid_size = 50;
int grid_offx = 20;
int grid_offy = 20;
SDL_Surface* scr = NULL;


struct Component {
	int x=0, y=0;
	int gridx=0, gridy=0;
	virtual int draw() const { return 0; }
};
struct C_start : Component {
	int x=3, y=3;
	virtual int draw() const {
		int xx = gridx * grid_size + grid_offx + x;
		int yy = gridy * grid_size + grid_offx + y;
		SDL_Rect r={ short(xx), short(yy), 5*8+3, 11};
		SDL_FillRect(scr, &r, gfx::drawc(200,0,0));
		gfx::drawstr(scr, xx+2, yy+2, "start");
		return 0;
	}
};
struct C_end : Component {
	int x=3, y=3;
	virtual int draw() const {
		int xx = gridx * grid_size + grid_offx + x;
		int yy = gridy * grid_size + grid_offy + y;
		SDL_Rect r={ short(xx), short(yy), 5*8+3, 11};
		SDL_FillRect(scr, &r, gfx::drawc(0,0,200));
		gfx::drawstr(scr, xx+2, yy+2, " end");
		return 0;
	}
};
vector<shared_ptr<Component>> clist;


int pollkeys() {
	SDL_Event e;
	while (SDL_PollEvent(&e)) {
		if (e.type==SDL_QUIT)  return 1;
		if (e.type==SDL_KEYDOWN || e.type==SDL_KEYUP) {
			// int dir=( e.type==SDL_KEYDOWN ? 1 : 0 );
			switch (e.key.keysym.sym) {
			case SDLK_ESCAPE:  return 1;
			default:  printf("key: %d\n", e.key.keysym.sym);
			}
		}
	}
	return 0;
}


int main(int argc, char** argv) {
	gfx::init(640, 480, "test");
	scr = SDL_GetVideoSurface();

	shared_ptr<Component> c = make_shared<C_start>();
	c->gridx = c->gridy = 0;
	clist.push_back(c);
	c = make_shared<C_end>();
	c->gridy = 2;
	clist.push_back(c);

	while (true) {
		SDL_FillRect(scr, NULL, gfx::drawc(0,0,0));

		// draw grid
		gfx::drawc(50,50,50);
		for (int i=grid_offx; i<640; i+=grid_size)
			gfx::drawline(scr, i, 0, i, 480);
		for (int i=grid_offy; i<480; i+=grid_size)
			gfx::drawline(scr, 0, i, 640, i);

		// components
		for (const auto c : clist)
			c->draw();

		// gfx::scale2x(scr, scr);
		if (pollkeys())  break;
		if (gfx::flip())  break;
	}

	return 0;
}
