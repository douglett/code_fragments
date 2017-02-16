#include <string>
#include <vector>
#include "pipelib.h"

using namespace std;


int DataBlock::cpos(int pos) {
	return  (pos >= 0 && pos < data.size());
}

uint32_t* DataBlock::to32(int pos) {
	if (!cpos(pos))  return NULL;
	return  (uint32_t*) &data[pos][0];
}

void DataBlock::setstr(int pos, const string& str) {
	if (!cpos(pos))  return;
	data[pos].resize(str.length() + 1);
	memcpy(&data[pos][0], str.c_str(), str.length() + 1);
}