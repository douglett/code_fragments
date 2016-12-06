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
	extern const lang_t 
		LANG_DEFAULT, 
		LANG_CPP,
		LANG_QBASIC;
	extern lang_t            lang;
	extern std::vector<tok>  toklist;
	int  parsefile (const std::string& fname);
	void show      (char fmt = 'l');
} // end tok