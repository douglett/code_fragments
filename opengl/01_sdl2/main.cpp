#include <iostream>
// #include <GL/glew.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <OpenGL/glu.h>
// #include "globj.h"
#include "gllib.h"

using namespace std;


// globals


int mkbox() {
	glbuild::make();
	glbuild::col (1,0,0);
	glbuild::tri ({ -1,-1,-1,  +1,-1,-1,  +1,+1,-1 });
	glbuild::tri ({ +1,+1,-1,  -1,+1,-1,  -1,-1,-1 });
	glbuild::col (0,0,1);
	glbuild::quad({ -1,-1,-1,  +1,-1,-1,  +1,-1,+1,  -1,-1,+1 });
	glbuild::col (1,1,0);
	glbuild::quad({ -1,-1,-1,  -1,-1,+1,  -1,+1,+1,  -1,+1,-1 });
	// GLobj* o = 
	glbuild::finalize();
	return 0;
}


int mkcam() {
	// build first camera
	glbuild::make();
	glbuild::col (1,0,0,0.5);
	glbuild::quad({ -1,-1,0,  +1,-1,0,  +1,+1,0,  -1,+1,0 });
	glbuild::tris({ 
		-1,-1,0,  +1,-1,0,  0,0,2,
		+1,-1,0,  +1,+1,0,  0,0,2,
		+1,+1,0,  -1,+1,0,  0,0,2,
		-1,+1,0,  -1,-1,0,  0,0,2,
	});
	GLobj* o = glbuild::finalize();
	o->translate(-4,4,0);
	// clone second camera
	o = globj::clone(o);
	o->translate(4,0,0);
	o->rotate(0,1,0,90);
	// clone third camera
	o = globj::clone(o);
	o->translate(0,-6,0);
	o->rotate(1,0,0,90);
	return 0;
}


int main() {
	printf("start\n");
	if (gllib::init())  return 1;

	mkbox();
	mkcam();
	
	float  rotspeed = 2,  box_rot = 0,  cam_rot = 0;

	while (gllib::running) {
		// apply camera rotation
		gllib::cam.rotate(0, 1, 0, cam_rot);

		// rotate box
		box_rot += rotspeed;
		globj::objlist[0].rotate(0, 1, 0, box_rot);
		globj::objlist[1].rotate(0, 1, 0, box_rot);  // also rotate camera 1

		// redraw
		gllib::paint();

		// get keys
		for (auto k : gllib::getkeys())
			switch (k) {
			case SDLK_ESCAPE:  gllib::running = 0;  break;
			case SDLK_LEFT:    cam_rot += rotspeed * -1;  break;
			case SDLK_RIGHT:   cam_rot += rotspeed * +1;  break;
			}
	}

	SDL_Quit();
}