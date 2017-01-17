/**
 * adapted from:
 *   http://www.opengl-tutorial.org/
*/
#pragma once

#include <vector>
#include <GL/glew.h>

namespace common {
	extern GLuint  vbufferID, cbufferID;
	int    setCol(GLfloat r, GLfloat g, GLfloat b);
	int    addVertex(GLfloat x, GLfloat y, GLfloat z);
	int    addVertexCol(GLfloat x, GLfloat y, GLfloat z,  GLfloat r, GLfloat g, GLfloat b);
	int    addTri(const std::vector<GLfloat>& tri);
	int    sendVertexes();
	int    clearVertexes();
	GLuint vbufferSize();
} // end common