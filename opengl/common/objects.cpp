#include <vector>
#include <GL/glew.h>
#include "objects.h"

using namespace std;


namespace common {

	static vector<GLfloat>  vert, color;
	static GLuint  vbufferlen = 0;
	static GLfloat  colr = 0.8f, colg = 0.8f, colb = 0.8f;
	GLuint  vbufferID = 0,  cbufferID = 0;

	int setCol(GLfloat r, GLfloat g, GLfloat b) {
		colr = r;
		colg = g;
		colb = b;
		return 0;
	}
	
	int addVertex(GLfloat x, GLfloat y, GLfloat z) {
		return  addVertexCol(x, y, z,  colr, colg, colb);
	}

	int addVertexCol(GLfloat x, GLfloat y, GLfloat z, GLfloat r, GLfloat g, GLfloat b) {
		vert.insert ( vert.end(),  { x, y, z } );
		color.insert( color.end(), { r, g, b } );
		return  vert.size();
	}

	int addTri(const vector<GLfloat>& tri) {
		if (tri.size() != 9) {
			fprintf(stderr, "wrong number of verticies in tri: %d\n", int(tri.size()));
			return 1;
		}
		addVertexCol( tri[0], tri[1], tri[2],   colr, colg, colb );
		addVertexCol( tri[3], tri[4], tri[5],   colr, colg, colb );
		addVertexCol( tri[6], tri[7], tri[8],   colr, colg, colb );
		return  0;
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

	int clearVertexes() {
		vert = color = {};
		return 0;
	}

	GLuint vbufferSize() {
		return  vbufferlen;
	}

} // end common