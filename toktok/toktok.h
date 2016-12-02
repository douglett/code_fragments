#pragma once

#include <string>
#include <vector>

namespace tok {
	enum STATE {
		ST_none,
		ST_ws,
		ST_ident,
		ST_num,
		ST_char,
		ST_string,
		ST_keyword,
		ST_comment
	};
	struct tok {
		STATE        state;
		std::string  str;
	};
	struct lang_t {
		std::string  lnstart, blstart, blend;  // comments
		std::vector<std::string>  keyw;
	};
	int  parsefile (const std::string& fname);
	void show      (char fmt = 'l');
	extern const lang_t 
		LANG_DEFAULT, 
		LANG_CPP;
	extern lang_t            lang;
	extern std::vector<tok>  toklist;
} // end tok

namespace util {
	enum col_code {
		// foreground
		RED      = 31,
		GREEN    = 32,
		YELLOW   = 33,
		BLUE     = 34,
		PURPLE   = 35,
		CYAN     = 36,
		GRAY     = 37,
		DEFAULT  = 39,
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