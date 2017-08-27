#include "swapdaemon.h"
#include "helpers/helpers.h"
#include <iostream>
#include <fstream>

using namespace std;


static int swap_index = 0;

struct Swapf {
	int index=0;
	fstream fs;
	vector<vector<char>> membox={{0},{0}};
	int mempos(int id) {
		int pos=0;
		for (int i=0; i<id; i++)
			pos+=membox[i].size();
		return pos;
	}
	Swapf() {
		index = swap_index++;
		string fname = helpers::strfmt("/tmp/dswapd/vterm%d.swap", index);
		fs.open(fname, fstream::out),  fs.close();  // make && clear file
		fs.open(fname, fstream::in | fstream::out | fstream::binary);
		// set aside header space
		membox[0].resize(128);
		// fs.seekp(128),  fs<<" ";
		fs.seekp(mempos(0));
		fs.write(&membox[0][0], membox[0].size());
		// set aside screen space
		membox[1].resize(320*240*sizeof(char));
		// fs.seekp(mempos(2)),  fs<<" ";
		fs.seekp(mempos(1));
		fs.write(&membox[1][0], membox[1].size());
	}
};

int init() {
	if (system("mkdir -p /tmp/dswapd"))  return 1;
	fstream fs("/tmp/dswapd/vterm0.swap", fstream::out);
	if (!fs.is_open())
		return fprintf(stderr, "could not create first swap file: /tmp/dswapd/vterm0.swap\n"), 1;
	swap_index++;
	return 0;
}


int main() {
	if (init())  return 1;
	Swapf swap;
}