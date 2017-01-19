#pragma once

#include <vector>
#include <OpenGL/gl.h>

struct GLvec3 { GLfloat  x, y, z; };
struct GLvec4 { GLfloat  x, y, z, a; };

struct GLtri {
public:
	GLfloat  vec[9]; // = {0};
	GLfloat  col[3]; // = {0};
};

class GLobj {
public:
	GLfloat  x=0,   y=0,   z=0;
	GLfloat  rx=0,  ry=0,  rz=0,  rot=0;
	std::vector<GLtri>  tris;
	void translate (GLfloat x, GLfloat y, GLfloat z);
	void rotate    (GLfloat x, GLfloat y, GLfloat z, GLfloat rot);
};

namespace globj {
	extern std::vector<GLobj>  objlist;
	GLobj* mkobj();
	GLobj* clone(const GLobj* obj);
	int    paintall();
} // end globj

namespace glbuild {
	void    make();
	GLobj*  finalize();
	void    col(float r, float g, float b);
	void    tri(std::vector<float> v);
}  // end glbuild