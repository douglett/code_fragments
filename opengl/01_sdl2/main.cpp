#include <iostream>
// #include <GL/glew.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <OpenGL/glu.h>
// #include "globj.h"
#include "gllib.h"

using namespace std;


// globals
GLobj* box = NULL;


int mkbox() {
	glbuild::make();
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
	o->rotate(90,0,1,0);
	// clone third camera
	o = gllib::mkcam();
	o->translate(0,-4,0);
	o->rotate(90,1,0,0);
	// make game box
	mkbox();
	cout << glbuild::serialize(box) << endl;
	
	float  rotspeed = 2,  box_rot = 0;
	printf("%f : %f %f %f \n", gllib::cam->rot, gllib::cam->rx, gllib::cam->ry, gllib::cam->rz);
	while (gllib::running) {
		// rotate box
		box_rot += rotspeed;
		box->rotate(box_rot, 0, 1, 0);
		// gllib::camlist[1].rotate(box_rot, 0, 1, 0);  // also rotate camera 1

		// redraw
		gllib::paint();

		// get keys
		for (auto k : gllib::getkeys())
			switch (k) {
			// quit
			case SDLK_ESCAPE:  gllib::running = 0;  break;
			// movement
			case SDLK_LEFT:    gllib::cam->rot += rotspeed * +1;  break;
			case SDLK_RIGHT:   gllib::cam->rot += rotspeed * -1;  break;
			// cameras
			case '1':  gllib::cam = &gllib::camlist[0];  break;
			case '2':  gllib::cam = &gllib::camlist[1];  break;
			case '3':  gllib::cam = &gllib::camlist[2];  break;
			}
	}

	SDL_Quit();
}