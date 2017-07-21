#pragma once

#include <string>
#include <vector>

namespace tokenizer {
	// data structures
	enum TOK_TYPE {
		TOK_NONE=0,
		TOK_STRING,
		TOK_CHAR,
		TOK_PUNC,
		TOK_IDENT,
		TOK_NUM,
		TOK_COMMENT
	};
	struct Token {
		std::string tok;
		int lineno, type;
	};
	extern std::vector<Token> tokens;
	// methods
	int parsefile(const std::string& fname);
	int showtokens();
	std::string type_name(int t);
} // end tokenizer