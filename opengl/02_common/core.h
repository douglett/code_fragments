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
#include "globj.h"

namespace common {
	extern int running;
	// extern std::vector<GLobj*> objlist;
	int init();
	int quit();
	std::shared_ptr<GLobj> makeGLobj();
	int paint();
} // end common