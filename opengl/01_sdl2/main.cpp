#include <iostream>
#include <cmath>
#include <SDL.h>
#include "gllib.h"

using namespace std;
using namespace x3;


// globals
GLobj  *box, *sky, *sfloor, *diamond;


int mkbox() {
	glbuild::make();
	glbuild::tex ("sine");
	glbuild::col (1,0,0);
	glbuild::tri ({ -1,-1,-1,  +1,-1,-1,  +1,+1,-1 });
	glbuild::tri ({ +1,+1,-1,  -1,+1,-1,  -1,-1,-1 });
	glbuild::col (0,0,1);
	glbuild::quad({ -1,-1,-1,  +1,-1,-1,  +1,-1,+1,  -1,-1,+1 });
	glbuild::col (1,1,0);
	glbuild::quad({ -1,-1,-1,  -1,-1,+1,  -1,+1,+1,  -1,+1,-1 });
	box = glbuild::finalize();
	box->translate(0,0.2,0);
	return 0;
}


int mkdiamond() {
	glbuild::make();
	glbuild::tex ("sine2");
	glbuild::col (1,0,0);
	glbuild::quad({ -1,-1,-1,  +1,-1,-1,  +1,+1,-1,  -1,+1,-1 });  // back
	glbuild::quad({ -1,-1,+1,  +1,-1,+1,  +1,+1,+1,  -1,+1,+1 });  // front
	glbuild::quad({ -1,-1,-1,  -1,-1,+1,  -1,+1,+1,  -1,+1,-1 });  // left
	glbuild::quad({ +1,-1,-1,  +1,-1,+1,  +1,+1,+1,  +1,+1,-1 });  // right
	glbuild::tri ({ -1,+1,-1,  +1,+1,-1,   0,3,0 });  // top 1
	glbuild::tri ({ -1,+1,-1,  -1,+1,+1,   0,3,0 });  // top 2
	glbuild::tri ({ +1,+1,+1,  -1,+1,+1,   0,3,0 });  // top 3
	glbuild::tri ({ +1,+1,+1,  +1,+1,-1,   0,3,0 });  // top 4
	glbuild::tri ({ -1,-1,-1,  +1,-1,-1,   0,-3,0 });  // bottom 1
	glbuild::tri ({ -1,-1,-1,  -1,-1,+1,   0,-3,0 });  // bottom 2
	glbuild::tri ({ +1,-1,+1,  -1,-1,+1,   0,-3,0 });  // bottom 3
	glbuild::tri ({ +1,-1,+1,  +1,-1,-1,   0,-3,0 });  // bottom 4
	diamond = glbuild::finalize();
	diamond->translate(2,0.5,2);
	diamond->scale(0.3);
	return 0;
}


int mkenv() {
	// sky box
	glbuild::make();
	glbuild::tex ("stars1");
	glbuild::col (1,1,1);
	glbuild::quad({ -1,-1,-1,  +1,-1,-1,  +1,+1,-1,  -1,+1,-1 });  // back
	glbuild::quad({ -1,-1,+1,  +1,-1,+1,  +1,+1,+1,  -1,+1,+1 });  // front
	glbuild::quad({ -1,-1,-1,  +1,-1,-1,  +1,-1,+1,  -1,-1,+1 });  // bottom
	glbuild::quad({ -1,+1,-1,  +1,+1,-1,  +1,+1,+1,  -1,+1,+1 });  // top
	glbuild::quad({ -1,-1,-1,  -1,-1,+1,  -1,+1,+1,  -1,+1,-1 });  // left
	glbuild::quad({ +1,-1,-1,  +1,-1,+1,  +1,+1,+1,  +1,+1,-1 });  // right
	sky = glbuild::finalize();
	sky->scale(20);
	// floor
	glbuild::make();
	glbuild::tex ("sine2");
	glbuild::col (0.8,0.8,0.8);
	glbuild::quad({ -1,0,-1,  +1,0,-1,  +1,0,+1,  -1,0,+1 });
	sfloor = glbuild::finalize();
	sfloor->scale(8);
	sfloor->translate(0,-1,0);
	return 0;
}


int mksquares() {
	GLsquare sq;
	sq.w = 512;
	sq.h = 70;
	sq.col[0] = 
	sq.col[1] = sq.col[2] = 0.2;
	gllib::squarelist.push_back(sq);
	// texture squares
	vector<string> texl = { "static", "stripes", "sine", "sine2", "stars1" };
	int x = 10;
	int sz = 50; //128
	for (const auto& s : texl) {
		GLsquare sq;
		sq.w = sq.h = sz;
		sq.y = 10;
		sq.x = x;
		x += sz + 10;
		sq.texID = gltex::gettexID(s);
		gllib::squarelist.push_back(sq);
	}
	return 0;
}


int main() {
	printf("start\n");
	if (gllib::init())  return 1;
	gllib::getcam(0)->translate(0,1,6);
	// clone second camera
	GLobj* o = gllib::mkcam();
	o->translate(4,1,0);
	o->yaw = 90;
	// clone third camera
	o = gllib::mkcam();
	o->translate(0,-4,0);
	o->pitch = 90;
	// make game objects
	gltex::generateall();  // all textures
	mkenv();
	mkbox();
	mkdiamond();
	mksquares();
	cout << glbuild::serialize(box) << endl;
	
	float  rotspeed = 2,  mspeed = 0.1;
	printf("yaw[%f]  pitch[%f]  roll[%f]\n", gllib::cam->yaw, gllib::cam->pitch, 0.0f);
	while (gllib::running) {
		// rotate box
		box->yaw     += rotspeed;
		diamond->yaw += rotspeed;

		// redraw
		gllib::paint();

		// get keys
		for (auto k : gllib::getkeys())
			switch (k) {
			// quit
			case SDLK_ESCAPE:  gllib::running = 0;  break;
			// movement
			case SDLK_LEFT:    gllib::cam->yaw += rotspeed;  break;
			case SDLK_RIGHT:   gllib::cam->yaw -= rotspeed;  break;
			case SDLK_UP:
			case SDLK_DOWN:
				{
					float z   = sin(RDEG * (gllib::cam->yaw + 270));
					float x   = sin(RDEG * (gllib::cam->yaw + 180));
					float dir = (k == SDLK_DOWN ? -1 : 1);
					if (abs(gllib::cam->z + z * mspeed * dir) < 7.6)  gllib::cam->z += z * mspeed * dir;
					if (abs(gllib::cam->x + x * mspeed * dir) < 7.6)  gllib::cam->x += x * mspeed * dir;
					// printf("%f  %f\n", z, x);
				}
				break;
			// cameras
			case '1':  gllib::cam = gllib::getcam(0);  break;
			case '2':  gllib::cam = gllib::getcam(1);  break;
			case '3':  gllib::cam = gllib::getcam(2);  break;
			}
	}

	SDL_Quit();
}