#include <vector>
#include <GL/glew.h>
#include "globj.h"

using namespace std;


namespace common {

	int GLobj::setCol(GLfloat r, GLfloat g, GLfloat b) {
		colr = r;
		colg = g;
		colb = b;
		return 0;
	}
	
	int GLobj::addVertex(GLfloat x, GLfloat y, GLfloat z) {
		vert.insert ( vert.end(),  { x,    y,    z    } );
		color.insert( color.end(), { colr, colg, colb } );
		return  vert.size();
	}

	int GLobj::addTri(const vector<GLfloat>& tri) {
		if (tri.size() != 9) {
			fprintf(stderr, "wrong number of verticies in tri: %d\n", int(tri.size()));
			return 1;
		}
		addVertex( tri[0], tri[1], tri[2] );
		addVertex( tri[3], tri[4], tri[5] );
		addVertex( tri[6], tri[7], tri[8] );
		return  0;
	}

	int GLobj::sendBuffer() {
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
		printf("sendBuffer   :: vbufferID  %d : cbufferID  %d : vbufferlen  %d \n", vbufferID, cbufferID, vbufferlen);
		return  vbufferlen;
	}

	int GLobj::deleteBuffer() {
		glDeleteBuffers(1, &vbufferID);
		glDeleteBuffers(1, &cbufferID);
		printf("deleteBuffer :: vbufferID  %d : cbufferID  %d \n", vbufferID, cbufferID);
		return  vbufferlen;
	}

	int GLobj::clearVertexes() {
		vert = color = {};
		return 0;
	}

	GLuint GLobj::bufferID(char t) {
		switch (t) {
		case 'v':   return vbufferID;
		case 'c':   return cbufferID;
		default:    return 0;
		}
	}

	GLuint GLobj::vbufferSize() {
		return  vbufferlen;
	}

} // end common