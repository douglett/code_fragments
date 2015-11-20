#include <iostream>
#include <string>
#include <sstream>

#include "tinylisp.h"

using namespace std;



// token class
Token::Token(int type, std::string val, int line, int pos) {
	this->type = type;
	this->val = val;
	this->line = line;
	this->pos = pos;
}

// val class
val::val() { }
val::val(int type) {
	this->type = type;
}



namespace lisp {

	// consts
	const val nil;

	// static
	static stringstream ss; // generic stringstream
	static int lerror_count = 0;

	// helper members
	int strtoint(string s) {
		ss.str(s), ss.clear();
		int i = 0;
		ss >> i;
		return i;
	}
	string inttostr(int i) {
		ss.str(""), ss.clear();
		ss << i;
		return ss.str();
	}
	string strtolower(const string& s) {
		string s2 = s;
		for (auto &c : s2)
			c = tolower(c);
		return s2;
	}

	// error handling
	void lerror(string type, string err, const Token* tok) {
		cerr << type << " error:: " << err;
		if (tok != NULL) 
			cerr << " [" << tok->line << ":" << tok->pos << "]";
		cerr << endl;
		lerror_count++;
	}
	int haserror() {
		return lerror_count;
	}

} // end tinylisp helpers
