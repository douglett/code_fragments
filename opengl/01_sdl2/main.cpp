#include <iostream>
#include <SDL.h>
#include "gllib.h"

using namespace std;
using namespace x3;


// globals
GLobj* box = NULL;


int mkbox() {
	glbuild::make();
	glbuild::tex ("stripes");
	glbuild::col (1,0,0);
	glbuild::tri ({ -1,-1,-1,  +1,-1,-1,  +1,+1,-1 });
	glbuild::tri ({ +1,+1,-1,  -1,+1,-1,  -1,-1,-1 });
	glbuild::col (0,0,1);
	glbuild::quad({ -1,-1,-1,  +1,-1,-1,  +1,-1,+1,  -1,-1,+1 });
	glbuild::col (1,1,0);
	glbuild::quad({ -1,-1,-1,  -1,-1,+1,  -1,+1,+1,  -1,+1,-1 });
	box = glbuild::finalize();
	box->translate(0,0,0);
	return 0;
}


int main() {
	printf("start\n");
	if (gllib::init())  return 1;
	// clone second camera
	GLobj* o = gllib::mkcam();
	o->translate(4,0,0);
	o->yaw = 90;
	// clone third camera
	o = gllib::mkcam();
	o->translate(0,-4,0);
	o->pitch = 90;
	// make textures
	gltex::generate("static",  "greyscale_static");
	gltex::generate("stripes", "greyscale_stripes");
	// make game box
	mkbox();
	cout << glbuild::serialize(box) << endl;
	
	float  rotspeed = 2;
	printf("yaw[%f]  pitch[%f]  roll[%f]\n", gllib::cam->yaw, gllib::cam->pitch, 0.0f);
	while (gllib::running) {
		// rotate box
		box->yaw += rotspeed;
		// gllib::camlist[1].rotate(box_rot, 0, 1, 0);  // also rotate camera 1

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
			// cameras
			case '1':  gllib::cam = gllib::getcam(0);  break;
			case '2':  gllib::cam = gllib::getcam(1);  break;
			case '3':  gllib::cam = gllib::getcam(2);  break;
			}
	}

	SDL_Quit();
}