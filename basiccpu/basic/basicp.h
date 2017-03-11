#pragma once

#include <string>
#include <vector>

namespace basic {
	// basicp
	extern std::vector<std::pair<std::string, int>>  funclist;
	extern std::vector<uint16_t>  head, fhead, dhead, body, prog;
	void showprog (const std::vector<uint16_t>& prog);
	// basicp_parsebas
	int  fparse   (const std::string& fname);
} // end basic