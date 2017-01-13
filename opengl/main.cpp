#include <iostream>
#include <SDL.h>
#include <SDL_opengl.h>
// #include <GL/glu.h>

using namespace std;


// globals
SDL_Window* win = NULL;
SDL_GLContext ctx;


int init() {
	// init sdl2
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "Failed to init SDL2\n");
		return 1;
	}
	// Create our window centered at 512x512 resolution
	win = SDL_CreateWindow(
		"My Game", 
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		512, 512,
		SDL_WINDOW_OPENGL );
	if (!win) {
		fprintf(stderr, "Failed to open SDL2 window\n");
		return 1;
	}
	// set GL attributes
	// SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK,   SDL_GL_CONTEXT_PROFILE_CORE );
	// SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION,  3 );
	// SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION,  2 );
	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER,           1 );
	ctx = SDL_GL_CreateContext(win);

	// set up perspective
	// glMatrixMode( GL_PROJECTION );  /* change to the projection matrix and set our viewing volume. */
	// glLoadIdentity();
	// gluPerspective( 45.0f, 1.0f, 0.1f, 100.0f );  /* Set our perspective */
	// glMatrixMode( GL_MODELVIEW );  /* Make sure we're chaning the model view and not the projection */
	// glLoadIdentity();
	
	return 0;
}


int draw() {
	glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    // glOrtho(-1, 1, 1, -1, 0, 10);
	// glTranslatef(-1.5f, 0.0f, -6.0f);  // Move Left 1.5 Units And Into The Screen 6.0
	// glTranslatef(0.5f, 0, 0);
	glTranslatef(0, 0, -0.9f);
	glRotatef(45, 0, 1, 0);
	glBegin(GL_TRIANGLES);                      // Drawing Using Triangles
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3f( 0.0f, 1.0f, 0.0f);          // Top
		glVertex3f(-1.0f,-1.0f, 0.0f);          // Bottom Left
		glVertex3f( 1.0f,-1.0f, 0.0f);          // Bottom Right
	glEnd();  	

	SDL_GL_SwapWindow(win);
	return 0;
}


int main() {
	printf("start\n");
	if (init())  return 1;

	draw();
	SDL_Delay(2000);

	SDL_Quit();
}