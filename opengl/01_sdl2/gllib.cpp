#include <iostream>
#include <SDL.h>
#include <SDL_opengl.h>
#include <OpenGL/glu.h>
#include "gllib.h"

using namespace std;


namespace gllib {
	static SDL_Window* win = NULL;
	static SDL_GLContext ctx;
	GLobj cam;
	
	int init() {
		// init sdl2
		if (SDL_Init(SDL_INIT_VIDEO) < 0) {
			fprintf(stderr, "Failed to init SDL2\n");
			return 1;
		}
		// Create our window centered at 512x512 resolution
		const auto  C = SDL_WINDOWPOS_CENTERED,  W = 512,  H = 512;
		win = SDL_CreateWindow( "My Game", C, C, W, H, SDL_WINDOW_OPENGL );
		if (!win) {
			fprintf(stderr, "Failed to open SDL2 window\n");
			return 1;
		}
		
		// set GL window attributes
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		ctx = SDL_GL_CreateContext(win);  // create window
		
		// set GL basic options
		glEnable(GL_DEPTH_TEST),  glDepthFunc(GL_LESS);  // stuff in background
		glEnable(GL_BLEND),       glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  // transparency

		// set up perspective
		glMatrixMode(GL_PROJECTION); 	// change to the projection matrix
		glLoadIdentity();  				// reset matrix to identity position
		gluPerspective(90.0f, float(W)/H, 0.1f, 100.0f);  // Set our perspective. this will show a warning that we can ignore
		glMatrixMode(GL_MODELVIEW); 	// Make sure we're changing the model from here on
		glLoadIdentity(); 				// reset matrix to identity position

		// set some defaults
		glClearColor(0.0, 0.0, 0.0, 1.0);
		globj::objlist.reserve(100);
		cam.translate(0, 0, 6);
		
		return 0;
	}

	int paint() {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// reset cam
		glLoadIdentity();  // clear matrix stack
		glRotatef( cam.rot, cam.rx, cam.ry, cam.rz );
		glTranslatef( -cam.x, -cam.y, -cam.z );
		glPushMatrix();
		// repaint
		globj::paintall();
		// flip
		return flip();
	}

	int flip() {
		SDL_GL_SwapWindow(win);
		return 0;
	}
} // end gllib