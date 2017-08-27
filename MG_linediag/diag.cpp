#include "graphics/graphics.h"
#include <iostream>
#include <vector>
#include <array>
#include <cassert>

using namespace std;


SDL_Surface* scr = NULL;

struct Grid {
	int size=50;
	int offx=20, offy=20;
	int cursx=0, cursy=0;
};
Grid grid;


void turtle(int x, int y, const vector<int>& pos) {
	assert(pos.size()%2 == 0);
	for (int i=0; i+1<pos.size(); i+=2) {
		gfx::drawline(scr, x, y, pos[i]+x, pos[i+1]+y);
		x+=pos[i], y+=pos[i+1];
	}
}
void mullines(int x, int y, const vector<array<int, 4>>& coords) {
	for (const auto& c : coords)
		gfx::drawline(scr, x+c[0], y+c[1], x+c[2], y+c[3]);
}


struct Component {
	int x=0, y=0;
	int gridx=0, gridy=0;
	int flag[100]={0};
	virtual int draw() const { return 0; }
	int pixx() const { return gridx * grid.size + grid.offx; }
	int pixy() const { return gridy * grid.size + grid.offy; }
	void dline(int x, int y, int d) const {
		switch (d) {
		case 0:  gfx::drawline(scr, x, y, x, y-23);  break;
		case 1:  gfx::drawline(scr, x, y, x+23, y);  break;
		case 2:  gfx::drawline(scr, x, y, x, y+23);  break;
		case 3:  gfx::drawline(scr, x, y, x-23, y);  break;
		}
	}
	void darrow(int x, int y, int d) const {
		switch (d) {
		case 0:  mullines(x, y-23, { {{0,0,-4,4}},  {{0,0,4,4}}  });  break;
		case 1:  mullines(x+23, y, { {{0,0,-4,-4}}, {{0,0,-4,4}} });  break;
		case 2:  mullines(x, y+23, { {{0,0,-4,-4}}, {{0,0,4,-4}} });  break;
		case 3:  mullines(x-23, y, { {{0,0,4,-4}},  {{0,0,4,4}}  });  break;
		}
	}
};
struct C_start : Component {
	// int x=3, y=3;
	virtual int draw() const {
		// pos
		int xx = gridx * grid.size + grid.offx;
		int yy = gridy * grid.size + grid.offx;
		// draw box
		SDL_Rect r={ short(xx+3), short(yy+3), 5*8+3, 11};
		SDL_FillRect(scr, &r, gfx::drawc(200,0,0));
		gfx::drawstr(scr, xx+5, yy+5, "start");
		// arrows out
		gfx::drawc(150,150,150);
		mullines(xx+grid.size/2, yy+15, {
			{{0,0,0,32}},
			{{0,32,-4,32-4}},
			{{0,32,+4,32-4}}
		});
		return 0;
	}
};
struct C_end : Component {
	virtual int draw() const {
		int xx = gridx * grid.size + grid.offx;
		int yy = gridy * grid.size + grid.offy;
		// box and text
		SDL_Rect r={ short(xx+3), short(yy+36), 5*8+3, 11};
		SDL_FillRect(scr, &r, gfx::drawc(0,0,200));
		gfx::drawstr(scr, xx+5, yy+36+2, " end");
		// arrows out
		gfx::drawc(150,150,150);
		mullines(xx+grid.size/2, yy+2, {
			{{0,0,0,32}},
			{{0,32,-4,32-4}},
			{{0,32,+4,32-4}}
		});
		return 0;
	}
};
struct C_split : Component {
	virtual int draw() const {
		// arrows out
		gfx::drawc(150,150,150);
		mullines(pixx()+grid.size/2, pixy()+2, {
			{{0,0,0,23}},
			{{0,23,-20,23}},
			{{-20,23,-20+4,23-4}},
			{{-20,23,-20+4,23+4}},
			{{0,23,20,23}},
			{{20,23,20-4,23-4}},
			{{20,23,20-4,23+4}}
		});
		gfx::drawstr(scr, pixx()+4, pixy()+34, "0");
		gfx::drawstr(scr, pixx()+40, pixy()+34, "1");
		return 0;
	}
};
struct C_join : Component {
	int dirin=1, dirout=2;
	virtual int draw() const {
		// arrow connection
		gfx::drawc(150,150,150);
		// mullines(xx+grid.size/2, yy+grid.size/2, {
		// 	{{-23,0,23,0}},
		// 	{{0,-23,0,23}}
		// });
		int xx = pixx() + grid.size/2;
		int yy = pixy() + grid.size/2;
		dline (xx, yy, dirin );
		dline (xx, yy, dirout);
		darrow(xx, yy, dirout);
		return 0;
	}
};
struct C_logic : Component {
	virtual int draw() const {
		int xx = pixx() + grid.size/2;
		int yy = pixy() + grid.size/2;
		int w = 8;
		// draw triangle
		switch (flag[0]) {
		case 1:
			mullines(xx, yy, {
				{{ -w,-w,w,-w }},
				{{ w,-w,0,w }},
				{{ -w,-w,0,w }}
			});
			break;
		}
		// draw outer arrows
		// dline (xx, yy, 0);
		// dline (xx, yy, 2);
		mullines(xx, yy, {
			{{ 0,-23,0,-w }},
			{{ 0,w,0,23 }}
		});
		darrow(xx, yy, 2);
		return 0;
	}
};
vector<shared_ptr<Component>> clist;


int pollkeys() {
	SDL_Event e;
	while (SDL_PollEvent(&e)) {
		if (e.type==SDL_QUIT)  return 1;
		if (e.type==SDL_KEYDOWN) {
			// int dir=( e.type==SDL_KEYDOWN ? 1 : 0 );
			switch (e.key.keysym.sym) {
			case SDLK_ESCAPE:  return 1;
			case SDLK_UP:     if (grid.cursy>0) grid.cursy--;  break;
			case SDLK_DOWN:   grid.cursy++;  break;
			case SDLK_LEFT:   if (grid.cursx>0) grid.cursx--;  break;
			case SDLK_RIGHT:  grid.cursx++;  break;
			// case 
			default:  printf("key: %d\n", e.key.keysym.sym);
			}
		}
	}
	return 0;
}


int main(int argc, char** argv) {
	gfx::init(640, 480, "test");
	scr = SDL_GetVideoSurface();

	// start
	shared_ptr<Component> c = make_shared<C_start>();
	c->gridx=2, c->gridy=0;
	clist.push_back(c);
	// end
	c = make_shared<C_end>();
	c->gridx=2, c->gridy=5;
	clist.push_back(c);
	// split
	c = make_shared<C_split>();
	c->gridx=2, c->gridy=1;
	clist.push_back(c);
	// join
	c = make_shared<C_join>();
	c->gridx=1, c->gridy=1;
	clist.push_back(c);
	// not
	c = make_shared<C_logic>();
	c->gridx=1, c->gridy=2, c->flag[0]=1;
	clist.push_back(c);

	SDL_Rect r;
	while (true) {
		SDL_FillRect(scr, NULL, gfx::drawc(0,0,0));

		// draw grid
		gfx::drawc(50,50,50);
		for (int i=grid.offx; i<640; i+=grid.size)
			gfx::drawline(scr, i, 0, i, 480);
		for (int i=grid.offy; i<480; i+=grid.size)
			gfx::drawline(scr, 0, i, 640, i);
		// current
		r.w=r.h=grid.size, r.x=grid.offx+grid.size*grid.cursx, r.y=grid.offy+grid.size*grid.cursy;
		SDL_FillRect(scr, &r, gfx::drawc(100,100,100));

		// components
		for (const auto c : clist)
			c->draw();

		// gfx::scale2x(scr, scr);
		if (pollkeys())  break;
		if (gfx::flip())  break;
	}

	return 0;
}
