#include <iostream>
#include "toktok.h"
#include "util.h"

using namespace std;


int main() {
	toktok::parsefile("util.cpp");
	toktok::show('c');
}