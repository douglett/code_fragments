#include "tokenizer.h"
#include <iostream>
#include <string>
#include <vector>

using namespace std;


static vector<string> args;


int main(int argc, char** argv) {
	for (int i=0; i<argc; i++)
		args.push_back(argv[i]);

	tokenizer::parsefile("tokenizer.cpp");
	tokenizer::showtokens();
}