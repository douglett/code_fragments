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
	int  parsefile (const std::string& fname);
	void show      (char fmt = 'l');
	static const std::vector<std::string> 
		KEYW_CPP = {
			"auto", "void", "int", "char",
			"struct", "enum", "const", "static", "if", "else", "return",
			"for", "while", "switch", "case", "break", "using", "namespace"
		};
	extern std::vector<tok>          toklist;
	extern std::vector<std::string>  keyw;
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