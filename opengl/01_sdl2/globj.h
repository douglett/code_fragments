#pragma once

#include <string>
#include <vector>

struct GLtri {
public:
	float  vec[9]; // = {0};
	float  col[4]; // = {0};
};

class GLobj {
public:
	std::string id;
	float  x=0,   y=0,   z=0;
	float  pitch=0,  yaw=0;
	std::vector<GLtri>  tris;
	void translate (float x, float y, float z);
	// void rotate    (float rot, float x, float y, float z);
};

namespace glbuild {
	void    make();
	GLobj*  finalize();
	GLobj*  clone(const GLobj* obj);
	void    col  (float r, float g, float b);
	void    col  (float r, float g, float b, float a);
	void    tri  (const std::vector<float>& v);
	void    quad (const std::vector<float>& v);
	void    tris (const std::vector<float>& v);
	void    quads(const std::vector<float>& v);
	std::string  serialize(const GLobj* obj);
	GLobj*       deserialize(const std::string& s);
}  // end glbuild