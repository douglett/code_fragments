#include <iostream>
#include <string>
#include <sstream>
#include "pipelib/pipelib.h"

using namespace std;


void showlibs();
int  sdl2_test();
void bmp_test();


int main() {
	printf("pipelib test\n");
	pipelib::dlpath = "./libs/";

	// showlibs();
	// bmp_test();
	return  sdl2_test();
}


template<typename ... Args>
string strfmt(const string& fmt, Args ... args) {
	int size = snprintf(NULL, 0, fmt.c_str(), args ...) + 1; // Extra space for '\0'
	char cs[size];
	snprintf(cs, size, fmt.c_str(), args ...);
	return cs;
}


void showlibs() {
	string cmd = "ls " + pipelib::dlpath + "*" + pipelib::ext;
	string res = pipelib::exec(cmd);
	// cout << res << endl;
	// get libs into vec array
	vector<string> vs;
	stringstream ss(res);
	string s;
	while (ss >> s) {
		s.replace(0, pipelib::dlpath.size(), "");
		s.replace(s.size() - pipelib::ext.size(), pipelib::ext.size(), "");
		vs.push_back(s);
	}
	for (auto& s : vs)
		printf("[%s]\n", s.c_str());
}


int sdl2_test() {
	pipelib::dblock_t data;
	if (pipelib::loadall({ "bmpload", "sdl2test" }))  return 1;

	// cout << strfmt("cls %02x%02x%02x%02x", 0x88, 0x00, 0x00, 0xff) << endl;
	// cout << strfmt("cls %08x", 0x880000ff) << endl;

	pipelib::send("sdl2test", "init");
	pipelib::send("sdl2test", "cls 0x880000ff");
	if (pipelib::send("bmpload", "load asd.bmp", &data))  return 1;  // load into slot 1
	pipelib::push32(data, { 20, 40 });  // blit data in slot 1, to pos in slot 2
	pipelib::send("sdl2test", "blit backbuffer", &data);  // do blit

	pipelib::send("sdl2test", "makesprite test1 20 20");
	pipelib::put32(data[1], { 0, 0 });
	pipelib::send("sdl2test", "blit test1", &data);
	pipelib::send("sdl2test", "sprite test1 x 0 y 20 z 1");
	int sx = 0;

	int running = 1;
	while (running) {
		pipelib::send("sdl2test", strfmt("sprite test1 x %d", sx++));
		if (pipelib::send("sdl2test", "flip"))  break;
		// handle keys
		pipelib::send("sdl2test", "getkeys", &data);
		int len = data[0].size() / sizeof(uint32_t);
		uint32_t* k = pipelib::get32(data[0]);
		for (int i = 0; i < len; i++)
			switch(k[i]) {
			case 27:  running = 0;  break;
			case 0x40000052:  break;  // up
			case 0x40000051:  break;  // down
			case 0x40000050:  break;  // left
			case 0x4000004f:  break;  // right
			default:  printf("%d  %x\n", k[i], k[i]);
			}
	}

	pipelib::unloadall();
	return 0;
}


void bmp_test() {
	pipelib::load("bmpload");
	// test basic functions
	pipelib::send("bmpload", "info");
	cout << pipelib::response << endl;
	pipelib::send("bmpload", "cmdlist");
	cout << pipelib::response << endl;
	// test bmp functions
	cout << "---" << endl;
	pipelib::send("bmpload", "load test.bmp");
	pipelib::unload("bmpload");
}