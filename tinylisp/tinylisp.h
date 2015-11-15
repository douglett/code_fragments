#ifndef __TINYLISP_DEF__
	#define __TINYLISP_DEF__ value

#include <string>
#include "token.h"
#include "val.h"

namespace tokens {
	int tokenize_file(std::string fname);
	void show();
}

namespace parser {
	val parse_lists(int &err);
	std::string show_val(const val& v);
	void show_list(const val &vlist, int tablen = 0);
}

val eval(const val& v);

#endif