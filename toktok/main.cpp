#include <iostream>
#include "toktok.h"

using namespace std;


int main() {
	tok::keyw = tok::KEYW_CPP;
	tok::parsefile("util.cpp");
	tok::show('c');
}