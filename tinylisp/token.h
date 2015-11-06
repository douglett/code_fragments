#ifndef __SYMBOL_DEF__
	#define __SYMBOL_DEF__

#include <string>

class Token {
public:
	int type = 0;
	std::string val;
	int line = 0;
	int pos = 0;
	Token(int type, std::string val, int line, int pos) {
		this->type = type;
		this->val = val;
		this->line = line;
		this->pos = pos;
	}
};

#endif