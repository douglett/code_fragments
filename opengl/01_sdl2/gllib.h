#pragma once

#include <vector>
#include <list>
#include "globj.h"

struct gvec3 { float  x, y, z; };
struct gvec4 { float  x, y, z, a; };

namespace gllib {
	extern std::list<GLobj>  camlist,  objlist;
	extern GLobj*  cam;
	extern int  running, showcam;
	int init();
	GLobj* mkcam();
	int paint();
	int paintobjs(const std::list<GLobj>& olist);
	int flip();
	const std::vector<uint32_t>& getkeys();
} // end gllib