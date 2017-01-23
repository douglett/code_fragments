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
GLobj* cam[3] = { NULL };


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


int mkcam() {
	// build first camera
	const float s = 0.7;
	glbuild::make();
	glbuild::col (1,0,0,0.5);
	glbuild::quad({ -s,-s,0,  +s,-s,0,  +s,+s,0,  -s,+s,0 });  // face
	glbuild::tris({   // sides
		-s,-s,0,  +s,-s,0,  0,0,2*s,
		+s,-s,0,  +s,+s,0,  0,0,2*s,
		-s,+s,0,  -s,-s,0,  0,0,2*s,
	});
	glbuild::col (0.0, 1.0, 0.0, 0.5);
	glbuild::tris({ +s,+s,0,  -s,+s,0,  0,0,2*s });  // top triangle
	GLobj* o = glbuild::finalize();
	o->translate(-4,4,0);
	cam[0] = o;
	// clone second camera
	o = globj::clone(o);
	o->translate(4,0,0);
	o->rotate(90,0,1,0);
	cam[1] = o;
	// clone third camera
	o = globj::clone(o);
	o->translate(0,-6,0);
	o->rotate(90,1,0,0);
	cam[2] = o;
	return 0;
}


int main() {
	printf("start\n");
	if (gllib::init())  return 1;
	mkbox(),  mkcam();
	cout << globj::serialize(box) << endl;
	
	float  rotspeed = 2,  box_rot = 0;
	int    cam_follow = -1;
	while (gllib::running) {
		// reapply camera position
		if (cam_follow > -1)  gllib::cam = *cam[cam_follow];

		// rotate box
		box_rot += rotspeed;
		// globj::objlist[0].rotate(box_rot, 0, 1, 0);
		box->rotate(box_rot, 0, 1, 0);
		cam[0]->rotate(box_rot, 0, 1, 0);  // also rotate camera 1

		// redraw
		gllib::paint();

		// get keys
		for (auto k : gllib::getkeys())
			switch (k) {
			// quit
			case SDLK_ESCAPE:  gllib::running = 0;  break;
			// movement
			// case SDLK_LEFT:    cam_rot += rotspeed * -1;  break;
			// case SDLK_RIGHT:   cam_rot += rotspeed * +1;  break;
			case SDLK_LEFT:    gllib::cam.rot += rotspeed * +1;  break;
			case SDLK_RIGHT:   gllib::cam.rot += rotspeed * -1;  break;
			// cameras
			case '1':
				gllib::cam.translate(0, 0, 6);
				gllib::cam.rotate(0, 0, 0, 0);
				cam_follow = -1;
				break;
			case '2':  gllib::cam = *cam[1];  cam_follow = -1;  break;
			case '3':  gllib::cam = *cam[2];  cam_follow = -1;  break;
			case '4':  cam_follow = 0;  break;
			}
	}

	SDL_Quit();
}