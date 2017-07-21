#include "tokenizer.h"
#include <iostream>
#include <string>

using namespace std;


int main(int argc, char** argv) {
	if (argc < 2)
		return fprintf(stderr, "expected argument: filename\n"), 1;
	int err = tokenizer::parsefile("tokenizer.cpp");
	if (err)  return err;
	tokenizer::showtokens();
	return 0;
}