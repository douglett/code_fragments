#include <iostream>
// #include <GL/glew.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <OpenGL/glu.h>

using namespace std;


// globals
int running = 1;
SDL_Window* win = NULL;
SDL_GLContext ctx;


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
	
	return 0;
}


int rot = 0;

int draw() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();  // reset matrix

	glTranslatef( 0, 0, -6 );
	glRotatef(rot, 0, -1, 0);
	glBegin(GL_TRIANGLES);                      // Drawing Using Triangles
		// glColor3f(1.0f, 0.0f, 0.0f);
		glColor4f(1.0f, 0.0f, 0.0f, 0.5f);
		glVertex3f( 0.0f, 1.0f, 0.0f);          // Top
		glVertex3f(-1.0f,-1.0f, 0.0f);          // Bottom Left
		glVertex3f( 1.0f,-1.0f, 0.0f);          // Bottom Right
	glEnd();

	glLoadIdentity();
	glTranslatef( 0, 0, -3 );
	glRotatef(rot+90, 0, -1, 0);
	glBegin(GL_TRIANGLES);                      // Drawing Using Triangles
		// glColor3f(0.0f, 0.0f, 1.0f);
		glColor4f(0.0f, 0.0f, 1.0f, 0.5f);
		glVertex3f( 0.0f, 1.0f, 0.0f);          // Top
		glVertex3f(-1.0f,-1.0f, 0.0f);          // Bottom Left
		glVertex3f( 1.0f,-1.0f, 0.0f);          // Bottom Right
	glEnd();
	// glLoadIdentity();

	SDL_GL_SwapWindow(win);
	return 0;
}


int main() {
	printf("start\n");
	if (init())  return 1;

	while (running) {
		rot++;
		draw();
		SDL_Delay(16);
		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			switch (e.type) {
			case SDL_QUIT:  running = 0;  break;
			case SDL_KEYDOWN:
				switch (e.key.keysym.sym) {
				case SDLK_ESCAPE:  running = 0;  break;
				default:  printf("%d\n", e.key.keysym.sym);  break;
				}
				break;
			}
		}
	}

	SDL_Quit();
}