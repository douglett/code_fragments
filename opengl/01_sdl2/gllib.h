#pragma once

#include <string>
#include <vector>
#include <list>
#include "globj.h"

namespace x3 {
	
struct gvec3 { float  x, y, z; };
struct gvec4 { float  x, y, z, a; };

namespace gllib {
	extern std::list<GLobj>  camlist,  objlist;
	extern GLobj*  cam;
	extern int  running, showcam;
	int init();
	int setPerspective(const std::string& per);
	int paint();
	int paintobjs(const std::list<GLobj>& olist);
	int flip();
	GLobj* mkcam();
	const std::vector<uint32_t>& getkeys();
	GLobj* getobj(int n);
	GLobj* getcam(int n);
	GLobj* getobj(const std::string& id);
	GLobj* getcam(const std::string& id);
} // end gllib
} // end x3