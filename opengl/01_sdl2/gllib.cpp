#include <iostream>
#include <SDL.h>
#include <SDL_opengl.h>
#include <OpenGL/glu.h>
#include "gllib.h"

using namespace std;


namespace gllib {
	static const int  WIN_W = 512,  WIN_H = 512;
	static SDL_Window*       win = NULL;
	static SDL_GLContext     ctx;
	static vector<uint32_t>  keys;
	list<GLobj>  camlist,  objlist;
	GLobj*       cam = NULL;
	int          running = 1,  showcam = 1;
	
	int init() {
		// init sdl2
		if (SDL_Init(SDL_INIT_VIDEO) < 0) {
			fprintf(stderr, "Failed to init SDL2\n");
			return 1;
		}
		// Create our window centered at 512x512 resolution
		const int  C = SDL_WINDOWPOS_CENTERED;
		win = SDL_CreateWindow( "My Game", C, C, WIN_W, WIN_H, SDL_WINDOW_OPENGL );
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
		glEnable(GL_TEXTURE_2D);  // allow textures

		// set some defaults
		glClearColor(0.0, 0.0, 0.0, 1.0);
		keys.reserve(64);
		cam = mkcam();  // first camera
		cam->translate(0, 0, 6);  // first cam position
		setPerspective("3d");  // set up perspective

		// simple opengl error checking
		GLenum error = glGetError();
		if (error != GL_NO_ERROR) {
			fprintf(stderr, "Error initialising OpenGL! %s\n", gluErrorString(error));
			return 1;
		}
		return 0;
	}

	int setPerspective(const string& per) {
		if (per == "2d") {
			glMatrixMode(GL_PROJECTION); 	// change to the projection matrix
			glLoadIdentity();
			glOrtho(0.0f, WIN_W, WIN_H, 0.0f, 0.0f, 1.0f);
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
		}
		if (per == "3d") {
			glMatrixMode(GL_PROJECTION); 	// change to the projection matrix
			glLoadIdentity();  				// reset matrix to identity position
			gluPerspective(90.0f, float(WIN_W)/WIN_H, 0.1f, 100.0f);  // Set our perspective. this will show a warning that we can ignore
			glMatrixMode(GL_MODELVIEW); 	// Make sure we're changing the model from here on
			glLoadIdentity(); 				// reset matrix to identity position
		}
		return 0;
	}

	static int paint2d() {
		static GLuint tex = 0;
		const int TEX_W = 128;
		// make texture
		if (tex == 0) {
			GLuint pixels[TEX_W * TEX_W];
			fill_n(pixels, TEX_W * TEX_W, 0xffff00ff);
			for (int y = 0; y < 20; y++)
				for (int x = 0; x < 20; x++)
					pixels[y * TEX_W + x] = 0xff0000ff;
			glGenTextures( 1, &tex );
			glBindTexture( GL_TEXTURE_2D, tex );
				glTexImage2D   ( GL_TEXTURE_2D, 0, GL_RGBA, TEX_W, TEX_W, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, pixels );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );  // simple pixel-perfect scaling
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );  // needed?
			glBindTexture( GL_TEXTURE_2D, 0 );
		}
		// glBegin(GL_QUADS);
			// glColor4f ( 1.0, 0.0, 0.0, 1.0 );
		// 	glVertex3f( 0, 0, z );
		// 	glVertex3f( w, 0, z );
		// 	glVertex3f( w, w, z );
		// 	glVertex3f( 0, w, z );
		// glEnd();
		glTranslatef(10, 10, 0);
		glColor4f ( 1.0, 1.0, 1.0, 1.0 );  // always reset to white before texture
		glBindTexture( GL_TEXTURE_2D, tex );
			glBegin(GL_QUADS);
				glTexCoord2f( 0, 0 );  glVertex2f( 0,     0 );
				glTexCoord2f( 1, 0 );  glVertex2f( TEX_W, 0 );
				glTexCoord2f( 1, 1 );  glVertex2f( TEX_W, TEX_W );
				glTexCoord2f( 0, 1 );  glVertex2f( 0,     TEX_W );
			glEnd();
		glBindTexture( GL_TEXTURE_2D, 0 );
		return 0;
	}

	int paint() {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();  // clear matrix stack
		// repaint ui objects
		setPerspective("2d");
		paint2d();
		// reset cam
		setPerspective("3d");
		glRotatef( cam->pitch,  -1, 0,0 );
		glRotatef( cam->yaw,     0,-1,0 );
		glTranslatef( -cam->x, -cam->y, -cam->z );
		// repaint 3d objects
		glPushMatrix();
			paintobjs(objlist);
			if (showcam)  paintobjs(camlist);
		glPopMatrix();
		// flip
		return flip();
	}

	int paintobjs(const list<GLobj>& olist) {
		// draw objects
		for (const auto& o : olist) {
			// glLoadIdentity();
			glPushMatrix();
			glTranslatef( o.x, o.y, o.z );
			glRotatef( o.yaw,   0,1,0 );
			glRotatef( o.pitch, 1,0,0 );
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
		return 0;
	}

	int flip() {
		SDL_GL_SwapWindow(win);
		return 0;
	}

	GLobj* mkcam() {
		const float s = 0.7;  // visible size
		// build it
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
		// add to cam list
		camlist.push_back( *glbuild::finalize() );
		objlist.pop_back();  // remove built item from objlist
		GLobj* cam = &camlist.back();
		cam->translate(0, 0, 0);  // center
		return cam;
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

	static GLobj* listpos(list<GLobj>& ls, int pos) {
		int i = 0;
		for (auto& o : ls) {
			if (i > pos)  break;
			if (i++ == pos)  return &o;
		}
		return NULL;  // just make the compiler happy
	}

	static GLobj* listid(list<GLobj>& ls, const string& id) {
		for (auto& o : ls)
			if (o.id == id)
				return &o;
		return NULL;
	}

	GLobj* getobj(int n) { return listpos(objlist, n); }
	GLobj* getcam(int n) { return listpos(camlist, n); }
	GLobj* getobj(const string& id) { return listid(objlist, id); }
	GLobj* getcam(const string& id) { return listid(camlist, id); }
} // end gllib