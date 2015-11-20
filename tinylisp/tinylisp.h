#ifndef __TINYLISP_DEF__
	#define __TINYLISP_DEF__

#include <string>
#include <vector>

// Token class - holds results of the tokenizer
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

// val class - hold all lisp values
class val {
public:
	// static
	enum TYPES {
		T_LIST,
		T_INT,
		T_IDENT,
		T_STRING
	};
	// member vars
	int type = 0;
	int ival = 0;
	std::string sval;
	std::vector<val> lval;
	const Token* tok = NULL;
	val() { }
	val(int type) {
		this->type = type;
	}
};

namespace tokens {
	int tokenize_file(std::string fname);
	void show();
}

namespace parser {
	val parse_lists(int &err);
	std::string show_val(const val& v);
	void show_list(const val &vlist, int tablen = 0);
}

namespace lisp {
	// consts
	extern const val nil;
	// helpers
	int strtoint(std::string s);
	std::string inttostr(int i);
	std::string strtolower(const std::string& s);
	void lerror(std::string type, std::string err, const Token* tok);
	int haserror();
	// member vars
	int isnil(const val& v);
	val eval(const val& v);
}

#endif