#pragma once

#include <string>
#include <vector>
#include "basiccpu.h"

namespace bc {
namespace parse {
	int  parse      (const std::vector<std::string>& tok, std::vector<uint16_t>& prog);
	int  load_parse (const std::string& fname, std::vector<uint16_t>& prog);
	void showprog   (const std::vector<uint16_t>& prog);
	int  load_parse (const std::string& fname, CPU& cpu);
	void showprog   (const CPU& cpu);	
} // end parse
} // end bc