#pragma once

#include <string>
#include <vector>

namespace basic {
	// basicp
	int  load     (const std::string& fname, std::vector<uint16_t>& prog);
	void showprog (const std::vector<uint16_t>& prog);
	// basicp2
	int  fparse   (const std::string& fname);
} // end basic