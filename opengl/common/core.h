/**
 * adapted from:
 *   http://www.opengl-tutorial.org/
*/
#pragma once

// GL Includes
#include <GL/glew.h>     // Include GLEW
#include <GLFW/glfw3.h>  // Include GLFW
#include <glm/glm.hpp>   // Include GLM
// local includes
#include "shader.h"
#include "objects.h"

namespace common {
	extern int running;
	int init();
	int quit();
	int paint();
} // end common