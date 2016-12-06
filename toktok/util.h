#pragma once

#include <string>

namespace util {
	enum col_code {
		// foreground
		RED         = 31,
		GREEN       = 32,
		YELLOW      = 33,
		BLUE        = 34,
		PURPLE      = 35,
		CYAN        = 36,
		GRAY        = 37,
		DEFAULT     = 39,
		// background
		BG_RED      = 41,
		BG_GREEN    = 42,
		BG_BLUE     = 44,
		BG_DEFAULT  = 49
	};
	std::string ccode      (col_code col);
	std::string escapews   (const std::string& s);
	std::string strtolower (std::string s);
} // end util