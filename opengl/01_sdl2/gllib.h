#pragma once

#include <string>
#include <vector>
#include <list>
#include "globj.h"
#include "gltex.h"

namespace x3 {

const float PI   = 3.141592,
            RDEG = PI / 180.0,
            FOV  = 75.0;
	
struct gvec3 { float  x, y, z; };
struct gvec4 { float  x, y, z, a; };

namespace gllib {
	extern std::list<GLobj>     camlist,  objlist;
	extern std::list<GLsquare>  squarelist;
	extern GLobj*  cam;
	extern int  running, showcam;
	int init();
	int setPerspective(const std::string& per);
	int paint();
	int paintobjs(const std::list<GLobj>& olist);
	int paintsquares();
	int flip();
	GLobj* mkcam();
	const std::vector<uint32_t>& getkeys();
	GLobj* getobj(int n);
	GLobj* getcam(int n);
	GLobj* getobj(const std::string& id);
	GLobj* getcam(const std::string& id);
} // end gllib
} // end x3