#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>

using namespace std;

// public
extern "C" {
	int buffercmd (const char* in, const char** out, void* data);
}
// private
static int loadbmp(const string& fname, vector<char>& vc);
typedef  vector<vector<char>>  data_t;


int buffercmd (const char* in, const char** out, void* data) {
	if (in == NULL || out == NULL)  return -1;
	// check command
	stringstream ss(in);
	string cmd;
	ss >> cmd;
	if (cmd == "info") {
		*out = "test bmp loader. returns  uint32_t [w, h, [rgba], ...]";
		return 0;
	} else if (cmd == "list") {
		*out = 
			"info :: library info\n"
			"list :: list commands\n"
			"load <filename> :: loads a file";
		return 0;
	} else if (cmd == "load") {
		if (data == NULL) {
			*out = "no data block provided";
			return 1;
		}
		auto* vc = (vector<vector<char>>*) data;
		*vc = { {} };  // reset data vector
		string fname;
		ss >> fname;
		// printf("loading file: %s\n", fname.c_str());
		if ( loadbmp(fname, vc->at(0)) ) {
			*out = "error loading file.";
			return 1;
		}
		return 0;
	}
	*out = "unknown command.";
	return 1;
}


struct BMPhead {
	uint32_t  off, w, h;
};
static BMPhead head;
static vector<char> data;
static int showinfo = 1;

static void header_1() {
	if (showinfo) {
		printf("BMP header  (data size: %d):\n", (int)data.size());
		printf("  sig :: %c%c\n", data[0], data[1]);
		printf("  siz :: %x  (%d)\n",  *((uint32_t*)&data[2]),  *((uint32_t*)&data[2]) );
		printf("   r1 :: %x\n", *((uint16_t*)&data[6]) );
		printf("   r1 :: %x\n", *((uint16_t*)&data[8]) );
		printf("  off :: %x  (%d)\n",  *((uint32_t*)&data[10]), *((uint32_t*)&data[10]) );
	}
	head.off = *((uint32_t*) &data[10]);
}

static void header_dib() {
	if (showinfo) {
		printf("DIB header:\n");
		printf("  siz :: %x  (%d)\n",  *((uint32_t*)&data[14]),  *((uint32_t*)&data[14]) );
		printf("    w :: %d\n", *((uint32_t*)&data[18]) );
		printf("    h :: %d\n", *((uint32_t*)&data[22]) );
		printf("  pln :: %d\n", *((uint16_t*)&data[26]) );
		printf("  bpp :: %d\n", *((uint16_t*)&data[28]) );
	}
	head.w = *((uint32_t*) &data[18]);
	head.h = *((uint32_t*) &data[22]);
}

static void loaddata(vector<char>& vc) {
	vc.resize((2 + head.w * head.h) * 4);
	*((uint32_t*)&vc[0]) = head.w;
	*((uint32_t*)&vc[4]) = head.h;
	for (int i = 0; i < head.w * head.h * 4; i++)
		vc[8 + i] = data[head.off + i];
}

static int loadbmp(const string& fname, vector<char>& vc) {
	// load data
	char d;
	fstream fs("test.bmp", fstream::in | fstream::binary);
	if (!fs.is_open())  return 1;
	while (fs >> d)
		data.push_back(d);
	fs.close();
	// show info
	header_1();
	header_dib();
	loaddata(vc);
	return 0;
}