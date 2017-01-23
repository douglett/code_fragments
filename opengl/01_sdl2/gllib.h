#pragma once

#include <vector>
#include "globj.h"

struct gvec3 { float  x, y, z; };
struct gvec4 { float  x, y, z, a; };

namespace gllib {
	extern std::vector<GLobj>  camlist,  objlist;
	extern GLobj cam;
	extern int running;
	int init();
	int paint();
	int paintobjs();
	int flip();
	const std::vector<uint32_t>& getkeys();
} // end gllib