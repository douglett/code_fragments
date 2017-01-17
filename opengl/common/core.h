/**
 * adapted from:
 *   http://www.opengl-tutorial.org/
*/
#pragma once

#include <GL/glew.h>

namespace common {
	extern int running;
	int init();
	int quit();
	int paint();
} // end common