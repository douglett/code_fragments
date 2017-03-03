#pragma once

#include <string>
#include <vector>
#include "basiccpu.h"

namespace bc {
namespace parse {
	int  load_parse (const std::string& fname, CPU& cpu);
} // end parse
} // end bc