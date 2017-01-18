/**
 * adapted from:
 *   http://www.opengl-tutorial.org/
*/
#pragma once

#include <vector>
#include <GL/glew.h>

namespace common {
	class non_copyable {
	protected:
		non_copyable()  = default;
		~non_copyable() = default;
		non_copyable(non_copyable const &)    = delete;
		void operator=(non_copyable const &x) = delete;
	};

	class GLobj : non_copyable {
	private:
		std::vector<GLfloat>  vert, color;
		GLuint  vbufferlen = 0;
		GLfloat  colr = 0.8f, colg = 0.8f, colb = 0.8f;
		GLuint  vbufferID = 0,  cbufferID = 0;
	public:
		int    setCol(GLfloat r, GLfloat g, GLfloat b);
		int    addVertex(GLfloat x, GLfloat y, GLfloat z);
		int    addTri(const std::vector<GLfloat>& tri);
		int    sendBuffer();
		int    deleteBuffer();
		int    clearVertexes();
		GLuint bufferID(char t);
		GLuint vbufferSize();
	};
} // end common