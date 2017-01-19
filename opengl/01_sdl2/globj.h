#pragma once

#include <vector>

struct GLtri {
public:
	float  vec[9]; // = {0};
	float  col[4]; // = {0};
};

class GLobj {
public:
	float  x=0,   y=0,   z=0;
	float  rx=0,  ry=0,  rz=0,  rot=0;
	std::vector<GLtri>  tris;
	void translate (float x, float y, float z);
	void rotate    (float x, float y, float z, float rot);
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
	void    col  (float r, float g, float b);
	void    col  (float r, float g, float b, float a);
	void    tri  (const std::vector<float>& v);
	void    quad (const std::vector<float>& v);
	void    tris (const std::vector<float>& v);
	void    quads(const std::vector<float>& v);
}  // end glbuild