#ifndef __VAL_DEF__
	#define __VAL_DEF__

#include <vector>

class val {
public:
	enum TYPES {
		T_LIST,
		T_INT,
		T_IDENT
	};
	int type = 0;
	int ival = 0;
	std::vector<val> lval;
	val() { }
	val(int type) {
		this->type = type;
	}
};

#endif