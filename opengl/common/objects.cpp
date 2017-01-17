#include <vector>
#include <GL/glew.h>
#include "objects.h"

using namespace std;


namespace common {

	static vector<GLfloat>  vert, color;
	static GLuint  vbufferlen = 0;
	GLuint  vbufferID = 0,  cbufferID = 0;
	
	int addVertex(GLfloat x, GLfloat y, GLfloat z) {
		return  addVertex(x, y, z,  0.8f, 0.8f, 0.8f);
	}

	int addVertex(GLfloat x, GLfloat y, GLfloat z, GLfloat r, GLfloat g, GLfloat b) {
		vert.insert ( vert.end(),  { x, y, z } );
		color.insert( color.end(), { r, g, b } );
		return  vert.size();
	}

	int sendVertexes() {
		// send vertex positions
		glGenBuffers(1, &vbufferID);
		glBindBuffer(GL_ARRAY_BUFFER, vbufferID);
		glBufferData(GL_ARRAY_BUFFER, (sizeof(GLfloat) * vert.size()), &vert[0], GL_STATIC_DRAW);
		// send vertex colors
		glGenBuffers(1, &cbufferID);
		glBindBuffer(GL_ARRAY_BUFFER, cbufferID);
		glBufferData(GL_ARRAY_BUFFER, (sizeof(GLfloat) * color.size()), &color[0], GL_STATIC_DRAW);
		// finish & report
		vbufferlen = vert.size();
		printf("vbufferID: %d    cbufferID: %d    vbufferlen: %d \n", vbufferID, cbufferID, vbufferlen);
		return  vbufferlen;
	}

	GLuint vbufferSize() {
		return  vbufferlen;
	}

} // end common