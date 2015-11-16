#include <iostream>
#include "tinylisp.h"

using namespace std;


int main() {
	int err = tokens::tokenize_file("doug2.lisp");
	if (err)
		return 1;
	// cout << ">> tokens:" << endl;
	// tokens::show(); 
	// cout << endl;

	val vlist = parser::parse_lists(err);
	if (err)
		return 1;
	// cout << ">> program list:" << endl;
	// parser::show_list(vlist);
	// cout << endl;

	cout << ">> parse:" << endl;
	for (auto v : vlist.lval) {
		auto rval = lisp::eval(v);
		// cout << parser::show_val(rval) << endl;
	}
	// cout << endl;
}
