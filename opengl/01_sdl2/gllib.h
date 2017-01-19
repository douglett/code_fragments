#pragma once

#include "globj.h"

struct gvec3 { float  x, y, z; };
struct gvec4 { float  x, y, z, a; };

namespace gllib {
	extern GLobj cam;
	int init();
	int paint();
	int flip();
} // end gllib