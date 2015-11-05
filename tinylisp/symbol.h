#ifndef __SYMBOL_DEF__
	#define __SYMBOL_DEF__

#include <string>

class Symbol {
public:
	int type = 0;
	std::string val;
	Symbol(int type, std::string val) {
		this->type = type;
		this->val = val;
	}
};

#endif