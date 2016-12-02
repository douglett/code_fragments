#pragma once

#include <string>
#include <vector>

namespace toktok {
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
	extern std::vector<tok> toklist;
	int  parsefile (const std::string& fname);
	void show      (char fmt = 'l');
} // end toktok