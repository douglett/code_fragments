#pragma once

#include <string>
#include <vector>

namespace basic {
	// basicp
	void showprog (const std::vector<uint16_t>& prog);
	// basicp_parseprog
	extern std::vector<uint16_t>  fhead, prog;
	int  fparse   (const std::string& fname);
	// basicp_pblock (test)
	int  pblock_test();
} // end basic