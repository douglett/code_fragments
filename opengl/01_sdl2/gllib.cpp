#include <iostream>
#include <SDL.h>
#include <SDL_opengl.h>
#include <OpenGL/glu.h>
#include "gllib.h"

using namespace std;


namespace gllib {
	static SDL_Window* win = NULL;
	static SDL_GLContext ctx;
	static vector<uint32_t> keys;
	std::vector<GLobj>  camlist,  objlist;
	GLobj cam;
	int running = 1;
	
	int init() {
		// init sdl2
		if (SDL_Init(SDL_INIT_VIDEO) < 0) {
			fprintf(stderr, "Failed to init SDL2\n");
			return 1;
		}
		// Create our window centered at 512x512 resolution
		const int  C = SDL_WINDOWPOS_CENTERED,  W = 512,  H = 512;
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
		keys.reserve(256);
		objlist.reserve(256);
		camlist.reserve(256);
		cam.translate(0, 0, 6);
		
		return 0;
	}

	int paint() {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// reset cam
		glLoadIdentity();  // clear matrix stack
		// glRotatef( cam.rot, cam.rx, cam.ry, cam.rz );
		glRotatef( cam.rot, -cam.rx, -cam.ry, -cam.rz );
		glTranslatef( -cam.x, -cam.y, -cam.z );
		glPushMatrix();
		// repaint objects
		paintobjs();
		// flip
		return flip();
	}

	int paintobjs() {
		// glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// draw objects
		for (const auto& o : objlist) {
			// glLoadIdentity();
			glPushMatrix();
			glTranslatef( o.x, o.y, o.z );
			glRotatef( o.rot, o.rx, o.ry, o.rz );
			// draw each tri
			for (const auto& t : o.tris) {
				const auto& c = t.col;  // col reference
				const auto& v = t.vec;  // vec reference
				glBegin(GL_TRIANGLES);
					glColor4f ( c[0], c[1], c[2], c[3] );
					glVertex3f( v[0], v[1], v[2] );
					glVertex3f( v[3], v[4], v[5] );
					glVertex3f( v[6], v[7], v[8] );
				glEnd();
			}
			glPopMatrix();
		}
		// swap backbuffer
		// SDL_GL_SwapWindow(win);
		return 0;
	}

	int flip() {
		SDL_GL_SwapWindow(win);
		return 0;
	}

	static int indexOf(const vector<uint32_t>& v, uint32_t k) {
		for (int i = 0; i < v.size(); i++)
			if (v[i] == k)
				return i;
		return -1;
	}

	const vector<uint32_t>& getkeys() {
		SDL_Event  e;
		uint32_t   sym;
		int        ii;
		while (SDL_PollEvent(&e)) {
			switch (e.type) {
			case SDL_QUIT:  running = 0;  break;
			case SDL_KEYDOWN:
				sym = e.key.keysym.sym;
				if (indexOf(keys, sym) == -1)  keys.push_back(sym);
				break;
			case SDL_KEYUP:
				sym = e.key.keysym.sym;
				if ((ii = indexOf(keys, sym)) > -1)   keys.erase(keys.begin() + ii);
				break;
			}
		}
		return keys;
	}
} // end gllib