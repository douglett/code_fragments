#include <iostream>
// #include <GL/glew.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <OpenGL/glu.h>
// #include "globj.h"
#include "gllib.h"

using namespace std;


// globals
int running = 1;
// SDL_Window* win = NULL;
// SDL_GLContext ctx;


// int init() {
// 	// init sdl2
// 	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
// 		fprintf(stderr, "Failed to init SDL2\n");
// 		return 1;
// 	}
// 	// Create our window centered at 512x512 resolution
// 	const auto  C = SDL_WINDOWPOS_CENTERED,  W = 512,  H = 512;
// 	win = SDL_CreateWindow( "My Game", C, C, W, H, SDL_WINDOW_OPENGL );
// 	if (!win) {
// 		fprintf(stderr, "Failed to open SDL2 window\n");
// 		return 1;
// 	}
	
// 	// set GL window attributes
// 	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
// 	ctx = SDL_GL_CreateContext(win);  // create window
	
// 	// set GL basic options
// 	glEnable(GL_DEPTH_TEST),  glDepthFunc(GL_LESS);  // stuff in background
// 	glEnable(GL_BLEND),       glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  // transparency

// 	// set up perspective
// 	glMatrixMode(GL_PROJECTION); 	// change to the projection matrix
// 	glLoadIdentity();  				// reset matrix to identity position
// 	gluPerspective(90.0f, float(W)/H, 0.1f, 100.0f);  // Set our perspective. this will show a warning that we can ignore
// 	glMatrixMode(GL_MODELVIEW); 	// Make sure we're changing the model from here on
// 	glLoadIdentity(); 				// reset matrix to identity position

// 	// set some defaults
// 	glClearColor(0.0, 0.0, 0.0, 1.0);
	
// 	return 0;
// }


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

	o = globj::clone(o);
	o->translate(4,0,0);
	o->rotate(0,1,0,90);
	o = globj::clone(o);
	o->translate(0,-6,0);
	o->rotate(1,0,0,90);

	return 0;
}


GLobj def;
GLobj cam;


int main() {
	printf("start\n");
	// if (init())  return 1;
	if (gllib::init())  return 1;
	mkbox();
	mkcam();
	auto& cam = gllib::cam;
	// make_obj();
	// cam.translate(0, 0, 6);
	// cam.rotate(1, 0, 0, 65.0);
	// def = cam;
	
	float rotspeed = 2, rot = 0, crot = 0;
	int xaxis = 0;

	while (running) {
		gllib::paint();

		// get keys
		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			switch (e.type) {
			case SDL_QUIT:  running = 0;  break;
			case SDL_KEYDOWN:
				switch (e.key.keysym.sym) {
				case SDLK_ESCAPE:  running = 0;  break;
				case SDLK_LEFT:    xaxis = -1;  break;
				case SDLK_RIGHT:   xaxis = +1;  break;
				
				case '1':  cam.translate(0, 0, 6);  crot = 0;    break;
				case '2':  cam.translate(0, 0, 4);  crot = 0;    break;
				case '3':  cam.translate(0, 0, 1);  crot = 0;    break;
				case '4':  cam.translate(4, 0, 0);  crot = 270;  break;
				
				default:  printf("%x\n", e.key.keysym.sym);  break;
				}
				break;
			case SDL_KEYUP:
				switch (e.key.keysym.sym) {
				case SDLK_LEFT:    xaxis = 0;  break;
				case SDLK_RIGHT:   xaxis = 0;  break;
				}
				break;
			}
		}

		// apply rotation
		crot += rotspeed * xaxis;
		cam.rotate(0, 1, 0, crot);
		rot += rotspeed;
		globj::objlist[0].rotate(0, 1, 0, rot);
		globj::objlist[1].rotate(0, 1, 0, rot);
	}

	SDL_Quit();
}