#include <iostream>
#include <SDL.h>
#include <SDL_opengl.h>
// #include <GL/glew.h>
// #include <OpenGL/gl.h>
// #include <OpenGL/glew.h>

using namespace std;


int main() {
	printf("start\n");
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "Failed to init SDL\n");
		return 1;
	}

	// Create our window centered at 512x512 resolution
	SDL_Window* win = SDL_CreateWindow(
		"My Game", 
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		512, 512,
		SDL_WINDOW_OPENGL );

	// set GL attributes
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK,   SDL_GL_CONTEXT_PROFILE_CORE );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION,  3 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION,  2 );
	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER,           1 );
	SDL_GLContext glctx = SDL_GL_CreateContext(win);
	// set up glew
	// glewExperimental = GL_TRUE;
	// glewInit();
	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS);

	// first draw
	glShadeModel(GL_SMOOTH);
	glClearColor(0, 0, 0.5, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(-1.5f, 0.0f, 2.0f);  // Move Left 1.5 Units And Into The Screen 6.0
	glBegin(GL_TRIANGLES);                      // Drawing Using Triangles
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3f( 0.0f, 1.0f, 0.0f);          // Top
		glVertex3f(-1.0f,-1.0f, 0.0f);          // Bottom Left
		glVertex3f( 1.0f,-1.0f, 0.0f);          // Bottom Right
	glEnd();  									// Finished Drawing The Triangle

	SDL_GL_SwapWindow(win);
	SDL_Delay(1000);

	SDL_Quit();
}