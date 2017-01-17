/**
 * adapted from:
 *   http://www.opengl-tutorial.org/
*/
#pragma once

#include <GL/glew.h>

namespace common {
	extern GLuint  vbufferID, cbufferID;
	int    addVertex(GLfloat x, GLfloat y, GLfloat z);
	int    addVertex(GLfloat x, GLfloat y, GLfloat z, GLfloat r, GLfloat g, GLfloat b);
	int    sendVertexes();
	GLuint vbufferSize();
} // end common