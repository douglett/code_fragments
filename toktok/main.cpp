#include <iostream>
#include "toktok.h"

using namespace std;


int main() {
	// tok::lang = tok::LANG_CPP;
	// tok::parsefile("util.cpp");
	tok::lang = tok::LANG_QBASIC;
	tok::parsefile("bas/nibbles.bas");
	tok::show('c');
}